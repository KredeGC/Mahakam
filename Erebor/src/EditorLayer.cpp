#include "ebpch.h"
#include "EditorLayer.h"

#ifndef MH_STANDALONE
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
#include "ConsoleLogSink.h"
#endif

#include <fstream>
#include <filesystem>

namespace Mahakam::Editor
{
	static Asset<Shader> blitShader;

	void EditorLayer::OnAttach()
	{
#ifndef MH_RUNTIME
		// Add the console panel to the logger
		auto sink = std::make_shared<ConsoleLogSinkMt>();

		Log::GetEngineLogger()->sinks().push_back(sink);
		Log::GetGameLogger()->sinks().push_back(sink);
#endif

		ComponentRegistry::RegisterDefaultComponents();

		AssetDatabase::LoadDefaultAssetImporters();

		AssetDatabase::ReloadAssetImports();

#pragma region Component properties
		// Transform
		PropertyRegistry::PropertyPtr transformInspector = [](Entity entity)
		{
			TransformComponent& transform = entity.GetComponent<TransformComponent>();

			bool isStatic = transform.IsStatic();
			if (ImGui::Checkbox("Static", &isStatic))
				transform.SetStatic(isStatic);

			glm::vec3 pos = transform.GetPosition();
			if (GUI::DrawVec3Control("Position", pos, 0.0f, isStatic))
				transform.SetPosition(pos);

			glm::vec3 eulerAngles = glm::degrees(transform.GetEulerAngles());
			if (GUI::DrawVec3Control("Rotation", eulerAngles, 0.0f, isStatic))
				transform.SetEulerangles(glm::radians(eulerAngles));

			glm::vec3 scale = transform.GetScale();
			if (GUI::DrawVec3Control("Scale", scale, 1.0f, isStatic))
				transform.SetScale(scale);
		};

		PropertyRegistry::Register("Transform", transformInspector);

		// Animator
		PropertyRegistry::PropertyPtr animatorInspector = [](Entity entity)
		{
			Animator& animator = entity.GetComponent<AnimatorComponent>();

			Asset<Animation> animation = animator.GetAnimation();

			if (animation)
			{
				float duration = animation->GetDuration() / animation->GetTicksPerSecond();

				ImGui::Text("Animation: %s", animation->GetName().c_str());
				ImGui::Text("Ticks per second: %d", animation->GetTicksPerSecond());
				ImGui::Text("Duration: %.1fs", duration);

				float progress = animator.GetTime() / animation->GetDuration();
				float realtime = animator.GetTime() / animation->GetTicksPerSecond();

				ImGui::ProgressBar(progress, ImVec2(-FLT_MIN, 0), std::to_string(realtime).c_str());
			}
		};

		PropertyRegistry::Register("Animator", animatorInspector);

		// AudioListener
		PropertyRegistry::PropertyPtr listenerInspector = [](Entity entity)
		{

		};

		PropertyRegistry::Register("Audio Listener", listenerInspector);

		// AudioSource
		PropertyRegistry::PropertyPtr audioSourceInspector = [](Entity entity)
		{
			AudioSourceComponent& source = entity.GetComponent<AudioSourceComponent>();
			Asset<Sound> sound = source.GetSound();

			std::filesystem::path importPath = sound.GetImportPath();
			if (GUI::DrawDragDropField("Sound", ".sound", importPath))
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

		PropertyRegistry::Register("Audio Source", audioSourceInspector);

		// Camera
		PropertyRegistry::PropertyPtr cameraInspector = [](Entity entity)
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

		PropertyRegistry::Register("Camera", cameraInspector);

		// Light
		PropertyRegistry::PropertyPtr lightInspector = [](Entity entity)
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

		PropertyRegistry::Register("Light", lightInspector);

		// Mesh
		PropertyRegistry::PropertyPtr meshInspector = [](Entity entity)
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
					// TODO: Show the material inspector instead?
					glm::vec3 color = material->GetFloat3("u_Color");
					if (ImGui::ColorEdit3("Color", glm::value_ptr(color)))
						material->SetFloat3("u_Color", color);
				}
			}
		};

		PropertyRegistry::Register("Mesh", meshInspector);
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
		Asset<Material> skyboxMaterial = Asset<Material>("import/assets/materials/Skybox.material.import");
		Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>("import/assets/textures/pines.irradiance.import");
		Asset<TextureCube> skyboxSpecular = Asset<TextureCube>("import/assets/textures/pines.specular.import");

		/*Asset<Shader> skyboxShader = Shader::Create("assets/shaders/Skybox.shader");
		Asset<Material> skyboxMaterial = Material::Create(skyboxShader);

		Asset<TextureCube> skyboxTexture = Asset<TextureCube>("import/assets/textures/pines.hdr.import");
		skyboxMaterial->SetTexture("u_Environment", 0, skyboxTexture);

		skyboxMaterial.Save("assets/shaders/Skybox.shader", "import/assets/materials/Skybox.material.import");

		AssetDatabase::ReloadAsset(skyboxMaterial.GetID());*/

		Ref<Scene> activeScene = Scene::Create();
		activeScene->SetSkyboxMaterial(skyboxMaterial);
		activeScene->SetSkyboxIrradiance(skyboxIrradiance);
		activeScene->SetSkyboxSpecular(skyboxSpecular);

		SceneManager::SetActiveScene(activeScene);


		// Load the runtime
#if MH_PLATFORM_WINDOWS
		m_Runtime = CreateScope<SharedLibrary>("runtime/Sandbox.dll");
#else
		m_Runtime = CreateScope<SharedLibrary>("runtime/libSandbox.so");
#endif

		auto runPtr = m_Runtime->GetFunction<void, Scene*>("Run");
		m_UpdatePtr = m_Runtime->GetFunction<void, Scene*, Timestep>("Update");

		// Run the game
		runPtr(activeScene.get());
	}

	void EditorLayer::OnDetach()
	{
		// Stop the game
		auto stopPtr = m_Runtime->GetFunction<void, Scene*>("Stop");
		stopPtr(SceneManager::GetActiveScene().get());
		
		// IMPORTANT: Unload the scene before unloading the runtime
		SceneManager::SetActiveScene(nullptr);

		// Unload the runtime
		m_Runtime = nullptr;
		m_UpdatePtr = nullptr;
		blitShader = nullptr;

		ComponentRegistry::DeregisterDefaultComponents();

		AssetDatabase::UnloadDefaultAssetImporters();
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		UpdateRuntimeLibrary();

		// Call shared library update
		m_UpdatePtr(SceneManager::GetActiveScene().get(), dt);

#ifdef MH_RUNTIME
		// TODO: Remove this, as the Sandbox project itself will compile as an executable instead
		// Only used during runtime
		SceneManager::GetActiveScene()->OnUpdate(dt);

		Renderer::GetFrameBuffer()->Blit(m_Framebuffer, true, false);
#else
		static const bool m_PlayMode = false;
		if (m_PlayMode)
			SceneManager::GetActiveScene()->OnUpdate(dt);
		else
			SceneManager::GetActiveScene()->OnUpdate(dt, true); // TEMPORARY until play-mode is implemented
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
					SceneManager::SetActiveScene(nullptr);
					//s_ActiveScene = nullptr;
					m_Runtime = nullptr;

					// Copy new runtime
					CopyRuntime(binaryStream, binaryLength);

					// Create new scene
					Ref<Scene> activeScene = Scene::Create("assets/textures/pines.hdr");
					SceneManager::SetActiveScene(activeScene);

					// Load new runtime
					m_Runtime = CreateScope<SharedLibrary>("runtime/Sandbox-runtime.dll");

					auto runPtr = m_Runtime->GetFunction<void, Scene*>("Run");

					runPtr(activeScene.get());

					m_UpdatePtr = m_Runtime->GetFunction<void, Scene*, Timestep>("Update");
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
		if (event.GetKeyCode() == Key::F5)
			Renderer::EnableWireframe(!Renderer::HasWireframeEnabled());

		if (event.GetKeyCode() == Key::F6)
			Renderer::EnableGBuffer(!Renderer::HasGBufferEnabled());

		if (event.GetKeyCode() == Key::F7)
			Renderer::EnableBoundingBox(!Renderer::HasBoundingBoxEnabled());

		return false;
	}

	bool EditorLayer::OnWindowResized(WindowResizeEvent& event)
	{
		SceneManager::GetActiveScene()->OnViewportResize(event.GetWidth(), event.GetHeight());

		return false;
	}
}