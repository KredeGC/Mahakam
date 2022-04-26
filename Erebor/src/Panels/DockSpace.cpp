#include "ebpch.h"
#include "DockSpace.h"

#include "EditorLayer.h"

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
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save...", "Ctrl+Shift+S"))
					SaveScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit"))
					Application::GetInstance().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				auto& windowProps = EditorWindowRegistry::GetWindowProps();
				for (auto& props : windowProps)
				{
					if (props.second.Viewable)
					{
						if (ImGui::MenuItem(props.second.Name.c_str()))
						{ }
					}
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
		bool controlPressed = Input::IsKeyPressed(MH_KEY_LEFT_CONTROL) || Input::IsKeyPressed(MH_KEY_RIGHT_CONTROL);
		bool shiftPressed = Input::IsKeyPressed(MH_KEY_LEFT_SHIFT) || Input::IsKeyPressed(MH_KEY_RIGHT_SHIFT);

		switch (event.GetKeyCode())
		{
		case MH_KEY_N:
			if (controlPressed)
				NewScene();
			break;
		case MH_KEY_O:
			if (controlPressed)
				OpenScene();
			break;
		case MH_KEY_S:
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
		EditorLayer::SetSelectedEntity({});
		Ref<Scene> scene = Scene::Create();
		EditorLayer::SetActiveScene(scene);
	}

	void DockSpace::OpenScene()
	{
		std::string filepath = FileUtility::OpenFile("Mahakam Scene (*.mhk)\0*.mhk\0");

		if (!filepath.empty())
		{
			EditorLayer::SetSelectedEntity({});
			Ref<Scene> scene = Scene::Create();
			EditorLayer::SetActiveScene(scene);

			SceneSerializer serializer(scene);
			serializer.Deserialize(filepath);
		}
	}

	void DockSpace::SaveScene()
	{

	}

	void DockSpace::SaveSceneAs()
	{
		std::string filepath = FileUtility::SaveFile("Mahakam Scene (*.mhk)\0*.mhk\0");

		if (!filepath.empty())
		{
			SceneSerializer serializer(EditorLayer::GetActiveScene());
			serializer.Serialize(filepath);
		}
	}
}