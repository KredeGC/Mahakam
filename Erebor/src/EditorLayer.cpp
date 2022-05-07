#include "ebpch.h"
#include "EditorLayer.h"

#include <fstream>
#include <filesystem>

namespace Mahakam::Editor
{
	static SharedLibrary* lib;

	static Ref<Shader> blitShader;

	void EditorLayer::OnAttach()
	{
		blitShader = Shader::Create("assets/shaders/internal/Blit.yaml");

		// Setup render passes for the default renderer
		Renderer::SetRenderPasses({
			CreateRef<GeometryRenderPass>(),
			CreateRef<LightingRenderPass>(),
			CreateRef<ParticleRenderPass>(),
			CreateRef<TonemappingRenderPass>() });

		// Create a new active scene
		s_ActiveScene = Scene::Create("assets/textures/pines.hdr");


#pragma region Components
		// Animator
		ComponentRegistry::ComponentInterface animatorInterface;
		animatorInterface.SetComponent<AnimatorComponent>();
		animatorInterface.OnInspector = [](Entity entity)
		{
			Animator& animator = entity.GetComponent<AnimatorComponent>();

			Ref<Animation> animation = animator.GetAnimation();

			float duration = animation->GetDuration() / animation->GetTicksPerSecond();

			ImGui::Text("Animation: %s", animation->GetName().c_str());
			ImGui::Text("Ticks per second: %d", animation->GetTicksPerSecond());
			ImGui::Text("Duration: %.1fs", duration);

			float progress = animator.GetTime() / animation->GetDuration();
			float realtime = animator.GetTime() / animation->GetTicksPerSecond();

			ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), std::to_string(realtime).c_str());
		};

		ComponentRegistry::RegisterComponent("Animator", animatorInterface);

		// AudioSource
		ComponentRegistry::ComponentInterface audioSourceInterface;
		audioSourceInterface.SetComponent<AudioSourceComponent>();
		audioSourceInterface.OnInspector = [](Entity entity)
		{
			AudioSourceComponent& source = entity.GetComponent<AudioSourceComponent>();
			Ref<Sound> sound = source.GetSound();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			if (sound)
				std::strncpy(buffer, source.GetSound()->GetFilePath().c_str(), sizeof(buffer));
			if (ImGui::InputText("##Filepath", buffer, sizeof(buffer)))
			{

			}

			float spatialBlend = source.GetSpatialBlend();
			if (ImGui::DragFloat("Spatial blend", &spatialBlend, 0.01f, 0.0f, 1.0f))
				source.SetSpatialBlend(spatialBlend);
		};

		ComponentRegistry::RegisterComponent("Audio Source", audioSourceInterface);

		// AudioListener
		ComponentRegistry::ComponentInterface audioListenerInterface;
		audioListenerInterface.SetComponent<AudioListenerComponent>();

		ComponentRegistry::RegisterComponent("Audio Listener", audioListenerInterface);

		// Camera
		ComponentRegistry::ComponentInterface cameraInterface;
		cameraInterface.SetComponent<CameraComponent>();
		cameraInterface.Serialize = [](YAML::Emitter& emitter, Entity entity)
		{
			CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			emitter << YAML::Key << "Projection" << YAML::Value << (int)camera.GetProjectionType();
			emitter << YAML::Key << "FOV" << YAML::Value << camera.GetFov();
			emitter << YAML::Key << "NearZ" << YAML::Value << camera.GetNearPlane();
			emitter << YAML::Key << "FarZ" << YAML::Value << camera.GetFarPlane();
			emitter << YAML::Key << "Size" << YAML::Value << camera.GetSize();
			emitter << YAML::Key << "Ratio" << YAML::Value << camera.GetRatio();
			emitter << YAML::Key << "FixedRatio" << YAML::Value << cameraComponent.HasFixedAspectRatio();

			return true;
		};
		cameraInterface.Deserialize = [](YAML::Node& node, Entity entity)
		{
			CameraComponent& cameraComponent = entity.AddComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			camera.SetProjectionType((Camera::ProjectionType)node["Projection"].as<int>());
			camera.SetFov(node["FOV"].as<float>());
			camera.SetNearPlane(node["NearZ"].as<float>());
			camera.SetFarPlane(node["FarZ"].as<float>());
			camera.SetSize(node["Size"].as<float>());
			camera.SetRatio(node["Ratio"].as<float>());
			cameraComponent.SetFixedAspectRatio(node["FixedRatio"].as<bool>());

			return true;
		};
		cameraInterface.OnInspector = [](Entity entity)
		{
			CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
			Camera& camera = cameraComponent;

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool selected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], selected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((Camera::ProjectionType)i);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.GetProjectionType() == Camera::ProjectionType::Perspective)
			{
				float fov = glm::degrees(camera.GetFov());
				if (ImGui::DragFloat("Field of view", &fov, 0.1f, 0.0f, 180.0f))
					camera.SetFov(glm::radians(fov));
			}
			else
			{
				float size = camera.GetSize();
				if (ImGui::DragFloat("Size", &size, 0.1f, 0.0f))
					camera.SetSize(size);
			}

			float nearClip = camera.GetNearPlane();
			if (ImGui::DragFloat("Near clip-plane", &nearClip, 0.1f, 0.0f))
				camera.SetNearPlane(nearClip);

			float farClip = camera.GetFarPlane();
			if (ImGui::DragFloat("Far clip-plane", &farClip, 0.1f, 0.0f))
				camera.SetFarPlane(farClip);

			bool fixedAspectRatio = cameraComponent.HasFixedAspectRatio();
			if (ImGui::Checkbox("Fixed aspect ratio", &fixedAspectRatio))
				cameraComponent.SetFixedAspectRatio(fixedAspectRatio);
		};

		ComponentRegistry::RegisterComponent("Camera", cameraInterface);

		// Mesh
		ComponentRegistry::ComponentInterface meshInterface;
		meshInterface.SetComponent<MeshComponent>();
		meshInterface.OnInspector = [](Entity entity)
		{
			MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();

			auto& meshes = meshComponent.GetMeshes();
			Ref<Material> material = meshComponent.GetMaterial();

			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
			for (auto& mesh : meshes)
			{
				if (mesh)
				{
					vertexCount += mesh->GetVertexCount();
					indexCount += mesh->GetIndexCount();
				}
			}

			if (vertexCount > 0 && indexCount > 0)
			{
				ImGui::Text("Vertex count: %d", vertexCount);
				ImGui::Text("Triangle count: %d", indexCount / 3);
			}

			if (material)
			{
				if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
				{
					// TODO: Temporary
					glm::vec3 color = material->GetFloat3("u_Color");
					if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
						material->SetFloat3("u_Color", color);
				}
			}
		};

		ComponentRegistry::RegisterComponent("Mesh", meshInterface);

		// Light
		ComponentRegistry::ComponentInterface lightInterface;
		lightInterface.SetComponent<LightComponent>();
		lightInterface.Serialize = [](YAML::Emitter& emitter, Entity entity)
		{
			Light& light = entity.GetComponent<LightComponent>();

			emitter << YAML::Key << "LightType" << YAML::Value << (int)light.GetLightType();
			emitter << YAML::Key << "Range" << YAML::Value << light.GetRange();
			emitter << YAML::Key << "FOV" << YAML::Value << light.GetFov();
			emitter << YAML::Key << "Color" << YAML::Value << light.GetColor();
			emitter << YAML::Key << "ShadowCasting" << YAML::Value << light.IsShadowCasting();
			emitter << YAML::Key << "ShadowBias" << YAML::Value << light.GetBias();

			return true;
		};
		lightInterface.Deserialize = [](YAML::Node& node, Entity entity)
		{
			Light& light = entity.AddComponent<LightComponent>();

			light.SetLightType((Light::LightType)node["LightType"].as<int>());
			light.SetRange(node["Range"].as<float>());
			light.SetFov(node["FOV"].as<float>());
			light.SetColor(node["Color"].as<glm::vec3>());
			light.SetShadowCasting(node["ShadowCasting"].as<bool>());
			light.SetBias(node["ShadowBias"].as<float>());

			return true;
		};
		lightInterface.OnInspector = [](Entity entity)
		{
			Light& light = entity.GetComponent<LightComponent>();

			const char* projectionTypeStrings[] = { "Directional", "Point", "Spot" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)light.GetLightType()];

			if (ImGui::BeginCombo("Light Type", currentProjectionTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool selected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], selected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						light.SetLightType((Light::LightType)i);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			float range = light.GetRange();
			if (ImGui::DragFloat("Range", &range, 0.1f, 0.0f, std::numeric_limits<float>::infinity()))
				light.SetRange(range);

			if (light.GetLightType() == Light::LightType::Spot)
			{
				float fov = glm::degrees(light.GetFov());
				if (ImGui::DragFloat("Field of view", &fov, 0.1f, 0.0f, 180.0f))
					light.SetFov(glm::radians(fov));
			}

			glm::vec3 color = light.GetColor();
			if (GUI::DrawColor3Edit("Color", color, ImGuiColorEditFlags_HDR))
				light.SetColor(color);

			bool hasShadows = light.IsShadowCasting();
			if (ImGui::Checkbox("Shadow casting", &hasShadows))
				light.SetShadowCasting(hasShadows);

			if (hasShadows)
			{
				float bias = light.GetBias();
				if (ImGui::DragFloat("Shadow bias", &bias, 0.001f, 0.0f, 1.0f))
					light.SetBias(bias);
			}
		};

		ComponentRegistry::RegisterComponent("Light", lightInterface);
#pragma endregion


#pragma region Windows
#ifndef MH_RUNTIME
		// GameViewPanel
		EditorWindowRegistry::EditorWindowProps gameViewPanelProps;
		gameViewPanelProps.Name = "Game View";
		gameViewPanelProps.SetWindow<GameViewPanel>();

		EditorWindowRegistry::RegisterWindow(gameViewPanelProps);

		EditorWindowRegistry::OpenWindow("Game View");

		// ProfilerPanel
		EditorWindowRegistry::EditorWindowProps profilerPanelProps;
		profilerPanelProps.Name = "Profiler";
		profilerPanelProps.SetWindow<ProfilerPanel>();

		EditorWindowRegistry::RegisterWindow(profilerPanelProps);

		EditorWindowRegistry::OpenWindow("Profiler");

		// RenderPassPanel
		EditorWindowRegistry::EditorWindowProps renderpassPanelProps;
		renderpassPanelProps.Name = "Renderpass";
		renderpassPanelProps.SetWindow<RenderPassPanel>();

		EditorWindowRegistry::RegisterWindow(renderpassPanelProps);

		EditorWindowRegistry::OpenWindow("Renderpass");

		// SceneHierarchyPanel
		EditorWindowRegistry::EditorWindowProps hierarchyPanelProps;
		hierarchyPanelProps.Name = "Scene Hierarchy";
		hierarchyPanelProps.SetWindow<SceneHierarchyPanel>();

		EditorWindowRegistry::RegisterWindow(hierarchyPanelProps);

		EditorWindowRegistry::OpenWindow("Scene Hierarchy");

		// SceneViewPanel
		EditorWindowRegistry::EditorWindowProps sceneViewPanelProps;
		sceneViewPanelProps.Name = "Scene View";
		sceneViewPanelProps.SetWindow<SceneViewPanel>();

		EditorWindowRegistry::RegisterWindow(sceneViewPanelProps);

		EditorWindowRegistry::OpenWindow("Scene View");

		// StatsPanel
		EditorWindowRegistry::EditorWindowProps statsPanelProps;
		statsPanelProps.Name = "Stats";
		statsPanelProps.SetWindow<StatsPanel>();

		EditorWindowRegistry::RegisterWindow(statsPanelProps);

		EditorWindowRegistry::OpenWindow("Stats");
#endif
#pragma endregion


#if MH_PLATFORM_WINDOWS
		lib = new SharedLibrary("runtime/Sandbox.dll");
#else
		lib = new SharedLibrary("runtime/libSandbox.so");
#endif

		auto runPtr = lib->GetFunction<void, Scene*>("Run");

		runPtr(s_ActiveScene.get());
	}

	void EditorLayer::OnDetach()
	{
		// IMPORTANT: Unload the scene before unloading the runtime
		s_ActiveScene = nullptr;

		delete lib;
		blitShader = nullptr;
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		UpdateRuntimeLibrary();

		// Call shared library update
		auto updatePtr = lib->GetFunction<void, Scene*, Timestep>("Update");

		updatePtr(s_ActiveScene.get(), dt);

#if MH_RUNTIME
		// Only used during runtime
		s_ActiveScene->OnUpdate(dt);

		blitShader->Bind("POSTPROCESSING");
		blitShader->SetTexture("u_Albedo", Renderer::GetFrameBuffer()->GetColorTexture(0));
		blitShader->SetUniformInt("u_Depth", 0);

		GL::EnableZTesting(false);
		GL::EnableZWriting(false);
		Renderer::DrawScreenQuad();
		GL::EnableZWriting(true);
		GL::EnableZTesting(true);
#else
		static const bool m_PlayMode = false;
		if (m_PlayMode)
			s_ActiveScene->OnUpdate(dt);
		else
			s_ActiveScene->OnUpdate(dt, true); // TEMPORARY until play-mode is implemented
#endif

		// Test compute shader
		/*debugComputeShader->Bind();
		debugComputeTexture->BindImage(0, false, true);
		debugComputeShader->Dispatch(std::ceil(width / 8), std::ceil(height / 4), 1);

		m_SceneViewPanel.SetFrameBuffer(debugComputeTexture);*/

#ifndef MH_RUNTIME
		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnUpdate(dt);
#endif
	}

	void EditorLayer::OnImGuiRender()
	{
		MH_PROFILE_RENDERING_FUNCTION();

#ifndef MH_RUNTIME
		m_DockSpace.Begin();

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnImGuiRender();

		m_DockSpace.End();
#endif
}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(EditorLayer::OnKeyPressed));

#ifndef MH_RUNTIME
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(m_DockSpace.OnKeyPressed));

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnEvent(event); // TODO: Fix to be blocking if true
#else
		dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(EditorLayer::OnWindowResized));
#endif
	}

	void EditorLayer::CopyRuntime(std::istream& binaryStream, size_t binaryLength)
	{
		// Copy new runtime
		std::vector<char> binaryBuffer;
		binaryBuffer.reserve(binaryLength);
		std::copy(std::istreambuf_iterator<char>(binaryStream),
			std::istreambuf_iterator<char>(),
			std::back_inserter(binaryBuffer));

		std::ofstream copyOut("runtime/Sandbox-runtime.dll", std::ios_base::binary);
		copyOut.write(binaryBuffer.data(), binaryLength);
		copyOut.close();
	}

	void EditorLayer::UpdateRuntimeLibrary()
	{
		if (std::filesystem::exists("runtime/Sandbox.dll"))
		{
			std::ifstream binaryStream("runtime/Sandbox.dll", std::ios_base::binary);

			binaryStream.seekg(0, std::ios_base::end);
			size_t binaryLength = binaryStream.tellg();
			binaryStream.seekg(0, std::ios_base::beg);

			if (std::filesystem::exists("runtime/Sandbox-runtime.dll"))
			{
				std::ifstream runtimeStream("runtime/Sandbox-runtime.dll", std::ios_base::binary);

				runtimeStream.seekg(0, std::ios_base::end);
				size_t runtimeLength = runtimeStream.tellg();
				runtimeStream.seekg(0, std::ios_base::beg);

				if (binaryLength != runtimeLength)
				{
					MH_CORE_TRACE("Reloading runtime code");

					// Unload old scene and runtime
					s_ActiveScene = nullptr;
					delete lib;

					// Copy new runtime
					CopyRuntime(binaryStream, binaryLength);

					// Create new scene
					s_ActiveScene = Scene::Create("assets/textures/pines.hdr");

					// Load new runtime
					lib = new SharedLibrary("runtime/Sandbox-runtime.dll");

					auto runPtr = lib->GetFunction<void, Scene*>("Run");

					runPtr(s_ActiveScene.get());
				}
			}
			else
			{
				MH_CORE_TRACE("Copying runtime code");

				// Copy new runtime
				CopyRuntime(binaryStream, binaryLength);
			}
		}
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == MH_KEY_F5)
			Renderer::EnableWireframe(!Renderer::HasWireframeEnabled());

		if (event.GetKeyCode() == MH_KEY_F6)
			Renderer::EnableGBuffer(!Renderer::HasGBufferEnabled());

		if (event.GetKeyCode() == MH_KEY_F7)
			Renderer::EnableBoundingBox(!Renderer::HasBoundingBoxEnabled());

		return false;
	}

	bool EditorLayer::OnWindowResized(WindowResizeEvent& event)
	{
		s_ActiveScene->OnViewportResize(event.GetWidth(), event.GetHeight());

		return false;
	}
}