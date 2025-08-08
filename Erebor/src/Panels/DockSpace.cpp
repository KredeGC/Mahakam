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
				if (ImGui::MenuItem(reinterpret_cast<const char*>(u8"\uec5b Open Project")))
				{
					std::filesystem::path projectPath = FileUtility::OpenDirectory();

					if (projectPath != "")
					{
						Application::GetInstance()->GetWindow().SetTitle("Erebor " + projectPath.string());

						FileUtility::SetProjectDirectory(projectPath);

						AssetDatabase::RefreshAssetPaths();
						ResourceRegistry::RefreshImportPaths();
					}
				}

				if (ImGui::MenuItem(reinterpret_cast<const char*>(u8"\uef1d Exit")))
					Application::GetInstance()->Close();

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
		case Key::O:
			
			break;
		default:
			break;
		}

		return false;
	}
}