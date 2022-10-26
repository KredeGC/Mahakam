#include "ebpch.h"
#include "DockSpace.h"

#include <imgui/imgui.h>

namespace Mahakam::Editor
{
	void DockSpace::Begin()
	{
		// Render dockspace
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace", nullptr, window_flags);

		ImGui::PopStyleVar(3);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWidth = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWidth;

		// File bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem(u8"\uec5b" " Open Project"))
				{
					std::filesystem::path projectPath = FileUtility::OpenDirectory();

					if (projectPath != "")
					{
						Application::GetInstance()->GetWindow().SetTitle("Erebor " + projectPath.string());

						FileUtility::ASSET_PATH = std::filesystem::relative(projectPath / "assets", FileUtility::GetWorkingDirectory());
						FileUtility::IMPORT_PATH = std::filesystem::relative(projectPath / "import", FileUtility::GetWorkingDirectory());

						NewScene();

						AssetDatabase::ReloadAssetImports();
					}
				}

				if (ImGui::MenuItem(u8"\uef1d" " Exit"))
					Application::GetInstance()->Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
				if (ImGui::MenuItem(u8"\uef10" " New Scene", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem(u8"\uec5b" " Open Scene...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem(u8"\uee00" " Save Scene...", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem(u8"\uee00" " Save Scene As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				auto& windowProps = EditorWindowRegistry::GetWindowProps();
				for (auto& props : windowProps)
				{
					if (props.second.Viewable)
					{
						bool enabled = false;
						if (props.second.Unique && props.second.Instance)
							enabled = true;

						if (ImGui::MenuItem(props.second.Name.c_str(), nullptr, enabled))
						{
							if (!enabled)
								EditorWindowRegistry::OpenWindow(props.second.Name);
							else if (props.second.Unique)
								EditorWindowRegistry::CloseWindow(props.second.Instance);
						}
					}
				}

				ImGui::EndMenu();
			}
            
            if (ImGui::BeginMenu("Build"))
            {
                if (ImGui::MenuItem("Build Assets"))
                {
                    // TODO: Build binary assets
                    
                    
                }
                
                if (ImGui::MenuItem("Build Executable"))
                {
                    // TODO: Build executable in MH_STANDALONE
                }
                
                ImGui::EndMenu();
            }

			ImGui::EndMenuBar();
		}
	}

	void DockSpace::End()
	{
		ImGui::End();
	}

	bool DockSpace::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 0) return false;

		// Shortcuts
		bool controlPressed = Input::IsKeyPressed(Key::LEFT_CONTROL) || Input::IsKeyPressed(Key::RIGHT_CONTROL);
		bool shiftPressed = Input::IsKeyPressed(Key::LEFT_SHIFT) || Input::IsKeyPressed(Key::RIGHT_SHIFT);

		switch (event.GetKeyCode())
		{
		case Key::N:
			if (controlPressed)
				NewScene();
			break;
		case Key::O:
			if (controlPressed)
				OpenScene();
			break;
		case Key::S:
			if (controlPressed && shiftPressed)
				SaveSceneAs();
			else if (controlPressed)
				SaveScene();
			break;
		default:
			break;
		}

		return false;
	}

	void DockSpace::NewScene()
	{
		Selection::SetSelectedEntity({});
		Ref<Scene> scene = Scene::Create();
		SceneManager::SetActiveScene(scene);
	}

	void DockSpace::OpenScene()
	{
		std::filesystem::path filepath = FileUtility::OpenFile("Mahakam Scene (*.mhk)\0*.mhk\0", FileUtility::GetWorkingDirectory() / FileUtility::ASSET_PATH);

		if (!filepath.empty())
		{
			Selection::SetSelectedEntity({});
			Ref<Scene> scene = Scene::Create();
			SceneManager::SetActiveScene(scene);

			SceneSerializer serializer(scene);
			serializer.Deserialize(filepath);
		}
	}

	void DockSpace::SaveScene()
	{

	}

	void DockSpace::SaveSceneAs()
	{
		std::filesystem::path filepath = FileUtility::SaveFile("Mahakam Scene (*.mhk)\0*.mhk\0", FileUtility::GetWorkingDirectory() / FileUtility::ASSET_PATH);

		if (!filepath.empty())
		{
			SceneSerializer serializer(SceneManager::GetActiveScene());
			serializer.Serialize(filepath);
		}
	}
}