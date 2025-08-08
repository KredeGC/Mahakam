#include "ebpch.h"
#include "ImporterLayer.h"

#include <Mahakam/Editor/Windows/ContentBrowserPanel.h>
#include <Mahakam/Editor/Windows/ImportWizardPanel.h>

#include <fstream>
#include <filesystem>

namespace Mahakam::Editor
{
	void ImporterLayer::OnAttach()
	{
		AssetDatabase::RegisterDefaultAssetImporters();

		AssetDatabase::RefreshAssetPaths();

		ResourceRegistry::RegisterDefaultAssetImporters();

		ResourceRegistry::RefreshImportPaths();

#pragma region Windows
		// ContentBrowserPanel
		EditorWindowRegistry::RegisterWindowClass<ContentBrowserPanel>("Content Browser");
		EditorWindowRegistry::OpenWindow("Content Browser");

		// ImportWizardPanel
		EditorWindowRegistry::RegisterWindowClass<ImportWizardPanel>("Import Wizard");
		EditorWindowRegistry::OpenWindow("Import Wizard");
#pragma endregion
	}

	void ImporterLayer::OnDetach()
	{
		AssetDatabase::DeregisterDefaultAssetImporters();

		ResourceRegistry::DeregisterDefaultAssetImporters();
	}

	void ImporterLayer::OnUpdate(Timestep dt)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnUpdate(dt);
	}

	void ImporterLayer::OnImGuiRender()
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

	void ImporterLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.DispatchEvent<KeyPressedEvent>(MH_BIND_EVENT(m_DockSpace.OnKeyPressed));

		auto& windows = EditorWindowRegistry::GetWindows();
		for (auto& window : windows)
			window->OnEvent(event); // TODO: Fix to be blocking if true
	}
}