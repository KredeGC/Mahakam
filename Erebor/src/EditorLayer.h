#pragma once

#include "Panels/DockSpace.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/StatsPanel.h"

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class EditorLayer : public Layer
	{
	private:
		Ref<Scene> activeScene;

		Entity cameraEntity;

		DockSpace dockSpace;
		ProfilerPanel profilerPanel;
		SceneViewPanel sceneViewPanel;
		SceneHierarchyPanel sceneHierarchyPanel;
		StatsPanel statsPanel;

		bool wireframe = false;

	public:
		EditorLayer() : Layer("Editor"), dockSpace(), profilerPanel(),
			sceneViewPanel(), sceneHierarchyPanel(), statsPanel() {}

		virtual void onAttach() override;
		virtual void onUpdate(Timestep dt) override;
		virtual void onImGuiRender() override;
		virtual void onEvent(Event& event) override;

		bool onKeyPressed(KeyPressedEvent& event);
		bool onWindowResize(WindowResizeEvent& event);
		bool onMouseScrolled(MouseScrolledEvent& event);
	};
}