#include "ebpch.h"
#include "EditorLayer.h"

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

#include "RenderPasses/BoundingBoxRenderPass.h"

#include <fstream>
#include <filesystem>

namespace Mahakam::Editor
{
	void EditorLayer::OnAttach()
	{
		// Add the console panel to the logger
		auto sink = CreateRef<ConsoleLogSinkMt>();

		Log::GetLogger()->sinks().push_back(sink);

		ComponentRegistry::RegisterDefaultComponents();

		AssetDatabase::RegisterDefaultAssetImporters();

		AssetDatabase::RefreshAssetPaths();

		ResourceRegistry::RegisterDefaultAssetImporters();

		ResourceRegistry::RefreshImportPaths();

#pragma region Windows
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
#pragma endregion

		// Setup render passes for the default renderer
		Renderer::SetRenderPasses({
			CreateRef<GeometryRenderPass>(),
			CreateRef<LightingRenderPass>(),
			CreateRef<ParticleRenderPass>(),
			CreateRef<BoundingBoxRenderPass>(),
			CreateRef<TonemappingRenderPass>() });

		// Use this once scenes are setup correctly
		Asset<Material> skyboxMaterial = Asset<Material>(1413214623375882268);
		Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>(5839156319952352249);
		Asset<TextureCube> skyboxSpecular = Asset<TextureCube>(14413137597893043784);

		Ref<Scene> activeScene = Scene::Create();
		activeScene->SetSkyboxMaterial(skyboxMaterial);
		activeScene->SetSkyboxIrradiance(skyboxIrradiance);
		activeScene->SetSkyboxSpecular(skyboxSpecular);

		SceneManager::SetActiveScene(activeScene);


		// Load the runtime
		Runtime::LoadRuntime("runtime", "Sandbox");

		// TEMP: Run the game, as we don't have a playmode yet
		Runtime::RunScene(activeScene);
	}

	void EditorLayer::OnDetach()
	{
		// TEMP: Stop the game, as we don't have a playmode yet
		Runtime::StopScene(SceneManager::GetActiveScene());
		
		// IMPORTANT: Unload the scene before unloading the runtime
		SceneManager::SetActiveScene(nullptr);

		// Unload the runtime
		Runtime::UnloadRuntime();

		ComponentRegistry::DeregisterDefaultComponents();

		AssetDatabase::DeregisterDefaultAssetImporters();

		ResourceRegistry::DeregisterDefaultAssetImporters();
	}

	void EditorLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		// Call runtime update
		Runtime::UpdateScene(SceneManager::GetActiveScene(), dt);

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

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnUpdate(dt);
	}

	void EditorLayer::OnImGuiRender()
	{
		MH_PROFILE_RENDERING_FUNCTION();

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
	}

	void EditorLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(EditorLayer::OnKeyPressed));

		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(m_DockSpace.OnKeyPressed));

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnEvent(event); // TODO: Fix to be blocking if true
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