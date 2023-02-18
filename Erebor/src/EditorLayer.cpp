#include "ebpch.h"
#include "EditorLayer.h"

#ifndef MH_STANDALONE
#include "Panels/AssetManagerPanel.h"
#include "Panels/BuildPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/EnvironmentPanel.h"
#include "Panels/ImportWizardPanel.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/RenderPassPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/StatsPanel.h"
#include "ConsoleLogSink.h"
#endif

#include "RenderPasses/BoundingBoxRenderPass.h"

#include <fstream>
#include <filesystem>

namespace Mahakam::Editor
{
	void EditorLayer::OnAttach()
	{
#ifndef MH_STANDALONE
		// Add the console panel to the logger
		auto sink = CreateRef<ConsoleLogSinkMt>();

		Log::GetEngineLogger()->sinks().push_back(sink);
		Log::GetGameLogger()->sinks().push_back(sink);
#endif

		ComponentRegistry::RegisterDefaultComponents();

		AssetDatabase::RegisterDefaultAssetImporters();

		AssetDatabase::ReloadAssets();

#pragma region Windows
#ifndef MH_STANDALONE
		// AssetManagerPanel
		EditorWindowRegistry::RegisterWindowClass<AssetManagerPanel>("Asset Manager");
		//EditorWindowRegistry::OpenWindow("Asset Manager");

		// ConsolePanel
		EditorWindowRegistry::RegisterWindowClass<BuildPanel>("Build Runtime");
		EditorWindowRegistry::OpenWindow("Build Runtime");

		// ConsolePanel
		EditorWindowRegistry::RegisterWindowClass<ConsolePanel>("Console");
		EditorWindowRegistry::OpenWindow("Console");

		// ContentBrowserPanel
		EditorWindowRegistry::RegisterWindowClass<ContentBrowserPanel>("Content Browser");
		EditorWindowRegistry::OpenWindow("Content Browser");

		// EnvironmentPanel
		EditorWindowRegistry::RegisterWindowClass<EnvironmentPanel>("Scene Environment");
		//EditorWindowRegistry::OpenWindow("Scene Environment");

		// ImportWizardPanel
		EditorWindowRegistry::EditorWindowProps panelProps;
		panelProps.Name = "Import Wizard";
		panelProps.Viewable = false;
		panelProps.SetWindow<ImportWizardPanel>();

		EditorWindowRegistry::RegisterWindow(panelProps);

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

		// Setup render passes for the default renderer
		Renderer::SetRenderPasses({
			CreateRef<GeometryRenderPass>(),
			CreateRef<LightingRenderPass>(),
			CreateRef<ParticleRenderPass>(),
			CreateRef<BoundingBoxRenderPass>(),
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


#ifndef MH_STANDALONE
		// Load the runtime
		Runtime::LoadRuntime("runtime", "Sandbox");

		// TEMP: Run the game, as we don't have a playmode yet
		Runtime::RunScene(activeScene);
#endif
	}

	void EditorLayer::OnDetach()
	{
#ifndef MH_STANDALONE
		// TEMP: Stop the game, as we don't have a playmode yet
		Runtime::StopScene(SceneManager::GetActiveScene());
#endif
		
		// IMPORTANT: Unload the scene before unloading the runtime
		SceneManager::SetActiveScene(nullptr);

#ifndef MH_STANDALONE
		// Unload the runtime
		Runtime::UnloadRuntime();
#endif

		ComponentRegistry::DeregisterDefaultComponents();

		AssetDatabase::DeregisterDefaultAssetImporters();
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_RENDERING_FUNCTION();

#ifndef MH_STANDALONE
		// Call runtime update
		Runtime::UpdateScene(SceneManager::GetActiveScene(), dt);
#endif

		static const bool m_PlayMode = false;
		if (m_PlayMode)
			SceneManager::GetActiveScene()->OnUpdate(dt);
		else
			SceneManager::GetActiveScene()->OnUpdate(dt, true); // TEMPORARY until play-mode is implemented

		// Test compute shader
		/*debugComputeShader->Bind();
		debugComputeTexture->BindImage(0, false, true);
		debugComputeShader->Dispatch(std::ceil(width / 8), std::ceil(height / 4), 1);

		m_SceneViewPanel.SetFrameBuffer(debugComputeTexture);*/

#ifndef MH_STANDALONE
		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnUpdate(dt);
#endif

		// Delete any entities marked for deletion
		SceneManager::GetActiveScene()->DestroyAllEntities<DeleteComponent>();
	}

	void EditorLayer::OnImGuiRender()
	{
		MH_PROFILE_RENDERING_FUNCTION();

#ifndef MH_STANDALONE
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
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(EditorLayer::OnKeyPressed));

#ifndef MH_STANDALONE
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(m_DockSpace.OnKeyPressed));

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnEvent(event); // TODO: Fix to be blocking if true
#else
		dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(EditorLayer::OnWindowResized));
#endif
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