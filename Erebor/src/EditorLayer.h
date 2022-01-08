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

		uint32_t* drawCalls;
		uint32_t* vertexCount;
		uint32_t* triCount;

		Ref<FrameBuffer> brdfFramebuffer;
		Ref<FrameBuffer> viewportFramebuffer;

		DockSpace dockSpace;
		ProfilerPanel profilerPanel;
		SceneViewPanel sceneViewPanel;
		SceneHierarchyPanel sceneHierarchyPanel;
		StatsPanel statsPanel;

		bool wireframe = false;

	public:
		EditorLayer(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
			: Layer("Editor"), drawCalls(drawCalls), vertexCount(vertexCount), triCount(triCount),
			dockSpace(), profilerPanel(), sceneViewPanel(), sceneHierarchyPanel(), statsPanel(drawCalls, vertexCount, triCount) {}

		virtual void onAttach() override;
		virtual void onUpdate(Timestep dt) override;
		virtual void onImGuiRender() override;
		virtual void onEvent(Event& event) override;

		bool onKeyPressed(KeyPressedEvent& event);
		bool onWindowResize(WindowResizeEvent& event);
		bool onMouseScrolled(MouseScrolledEvent& event);
	};
}