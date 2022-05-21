#include "ebpch.h"
#include "EditorLayer.h"
#include "ConsoleLogSink.h"

#ifndef MH_RUNTIME
#include "Panels/AssetManagerPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/GameViewPanel.h"
#include "Panels/ImportWizardPanel.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/RenderPassPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/StatsPanel.h"
#endif

#include <fstream>
#include <filesystem>

namespace Mahakam::Editor
{
	static SharedLibrary* lib;

	static Asset<Shader> blitShader;

	void EditorLayer::OnAttach()
	{
		// Add the console panel to the logger
		auto sink = std::make_shared<ConsoleLogSinkMt>();

		Log::GetEngineLogger()->sinks().push_back(sink);
		Log::GetGameLogger()->sinks().push_back(sink);

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
		audioSourceInterface.Serialize = [](YAML::Emitter& emitter, Entity entity)
		{
			AudioSourceComponent& source = entity.GetComponent<AudioSourceComponent>();

			emitter << YAML::Key << "Sound" << YAML::Value << source.GetSound().GetID();
			emitter << YAML::Key << "SpatialBlend" << YAML::Value << source.GetSpatialBlend();

			return true;
		};
		audioSourceInterface.Deserialize = [](YAML::Node& node, Entity entity)
		{
			AudioSourceComponent& source = entity.AddComponent<AudioSourceComponent>();

			YAML::Node soundNode = node["Sound"];
			if (soundNode)
			{
				Asset<Sound> sound = Asset<Sound>(soundNode.as<uint64_t>());

				if (sound)
				{
					source.SetSound(sound);
					source.Play(); // TODO: TEMPORARY, REMOVE WHEN PLAY MODE IS IMPL
				}
			}

			YAML::Node spatialNode = node["SpatialBlend"];
			if (spatialNode)
				source.SetSpatialBlend(spatialNode.as<float>());

			return true;
		};
		audioSourceInterface.OnInspector = [](Entity entity)
		{
			AudioSourceComponent& source = entity.GetComponent<AudioSourceComponent>();
			Asset<Sound> sound = source.GetSound();

			std::filesystem::path importPath = sound.GetImportPath();
			if (GUI::DrawDragDropTarget("Sound", ".sound", importPath))
			{
				source.SetSound(Asset<Sound>(importPath));
				source.Play(); // TODO: TEMPORARY, REMOVE WHEN PLAY MODE IS IMPL
			}

			float spatialBlend = source.GetSpatialBlend();
			if (ImGui::DragFloat("Spatial blend", &spatialBlend, 0.01f, 0.0f, 1.0f))
				source.SetSpatialBlend(spatialBlend);

			bool interpolate = source.GetInterpolation();
			if (ImGui::Checkbox("Interpolate", &interpolate))
				source.SetInterpolation(interpolate);
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
			Asset<Material> material = meshComponent.GetMaterial();

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


#pragma region Assets
		// Material
		Ref<MaterialAssetImporter> materialAssetImporter = CreateRef<MaterialAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".shader", materialAssetImporter);

		// Shader
		/*Ref<ShaderAssetImporter> shaderAssetImporter = CreateRef<ShaderAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".shader", shaderAssetImporter);*/

		// Sound
		Ref<SoundAssetImporter> soundAssetImporter = CreateRef<SoundAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".wav", soundAssetImporter);
		AssetDatabase::RegisterAssetImporter(".mp3", soundAssetImporter);

		// Texture
		Ref<TextureAssetImporter> textureAssetImporter = CreateRef<TextureAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".png", textureAssetImporter);
		AssetDatabase::RegisterAssetImporter(".jpeg", textureAssetImporter);
		AssetDatabase::RegisterAssetImporter(".jpg", textureAssetImporter);
		AssetDatabase::RegisterAssetImporter(".hdr", textureAssetImporter);
#pragma endregion


#pragma region Windows
#ifndef MH_RUNTIME
		// AssetManagerPanel
		EditorWindowRegistry::RegisterWindowClass<AssetManagerPanel>("Asset Manager");
		//EditorWindowRegistry::OpenWindow("Asset Manager");

		// ConsolePanel
		EditorWindowRegistry::RegisterWindowClass<ConsolePanel>("Console");
		EditorWindowRegistry::OpenWindow("Console");

		// ContentBrowserPanel
		EditorWindowRegistry::RegisterWindowClass<ContentBrowserPanel>("Content Browser");
		EditorWindowRegistry::OpenWindow("Content Browser");

		// ImportWizardPanel
		EditorWindowRegistry::EditorWindowProps panelProps;
		panelProps.Name = "Import Wizard";
		panelProps.Viewable = false;
		panelProps.SetWindow<ImportWizardPanel>();

		EditorWindowRegistry::RegisterWindow(panelProps);

		// GameViewPanel
		EditorWindowRegistry::RegisterWindowClass<GameViewPanel>("Game View");
		EditorWindowRegistry::OpenWindow("Game View");

		// ProfilerPanel
		EditorWindowRegistry::RegisterWindowClass<ProfilerPanel>("Profiler");
		//EditorWindowRegistry::OpenWindow("Profiler");

		// RenderPassPanel
		EditorWindowRegistry::RegisterWindowClass<RenderPassPanel>("Renderpass");
		//EditorWindowRegistry::OpenWindow("Renderpass");

		// SceneHierarchyPanel
		EditorWindowRegistry::RegisterWindowClass<SceneHierarchyPanel>("Scene Hierarchy");
		EditorWindowRegistry::OpenWindow("Scene Hierarchy");

		// SceneViewPanel
		EditorWindowRegistry::RegisterWindowClass<SceneViewPanel>("Scene View");
		EditorWindowRegistry::OpenWindow("Scene View");

		// StatsPanel
		EditorWindowRegistry::RegisterWindowClass<StatsPanel>("Stats");
		EditorWindowRegistry::OpenWindow("Stats");
#endif
#pragma endregion


		AssetDatabase::ReloadAssetImports();

		blitShader = Shader::Create("assets/shaders/internal/Blit.shader");

		// Setup render passes for the default renderer
		Renderer::SetRenderPasses({
			CreateRef<GeometryRenderPass>(),
			CreateRef<LightingRenderPass>(),
			CreateRef<ParticleRenderPass>(),
			CreateRef<TonemappingRenderPass>() });

		// Create a new active scene
		//s_ActiveScene = Scene::Create("assets/textures/pines.hdr");

		// Use this once scenes are setup correctly
		Asset<Material> skyboxMaterial = Asset<Material>("res/assets/materials/Skybox.material.yaml");
		Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>("res/assets/textures/pines.irradiance.yaml");
		Asset<TextureCube> skyboxSpecular = Asset<TextureCube>("res/assets/textures/pines.specular.yaml");

		/*Asset<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.shader");
		Asset<Material> skyboxMaterial = Material::Create(skyboxShader);

		Asset<TextureCube> skyboxTexture = Asset<TextureCube>("res/assets/textures/pines.hdr.yaml");
		skyboxMaterial->SetTexture("u_Environment", 0, skyboxTexture);

		skyboxMaterial.Save("assets/shaders/Skybox.shader", "res/assets/materials/Skybox.material.yaml");

		AssetDatabase::ReloadAsset(skyboxMaterial.GetID());*/

		s_ActiveScene = Scene::Create();
		s_ActiveScene->SetSkyboxMaterial(skyboxMaterial);
		s_ActiveScene->SetSkyboxIrradiance(skyboxIrradiance);
		s_ActiveScene->SetSkyboxSpecular(skyboxSpecular);


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

		delete lib; // TODO: Make this better, why the fuck is it just a static field
		blitShader = nullptr;

#pragma region Assets
		// Material
		AssetDatabase::DeregisterAssetImporter(".shader");

		// Shader
		//AssetDatabase::DeregisterAssetImporter(".shader");

		// Sound
		AssetDatabase::DeregisterAssetImporter(".wav");
		AssetDatabase::DeregisterAssetImporter(".mp3");

		// Texture
		AssetDatabase::DeregisterAssetImporter(".png");
		AssetDatabase::DeregisterAssetImporter(".jpeg");
		AssetDatabase::DeregisterAssetImporter(".jpg");
		AssetDatabase::DeregisterAssetImporter(".hdr");
#pragma endregion
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

		auto iter = windows.begin();
		while (iter != windows.end())
		{
			if (!(*iter)->IsOpen())
				iter = EditorWindowRegistry::CloseWindow(*iter);
			else
				iter++;
		}

		m_DockSpace.End();
#endif

#ifdef MH_ENABLE_PROFILING
		Profiler::ClearResults();
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