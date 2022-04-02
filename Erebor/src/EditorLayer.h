#pragma once

#include "Panels/DockSpace.h"
#include "Panels/GameViewPanel.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/StatsPanel.h"
#include "EditorCamera.h"

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class EditorLayer : public Layer
	{
	private:
		Ref<Scene> activeScene;

		DockSpace dockSpace;
		GameViewPanel gameViewPanel;
		ProfilerPanel profilerPanel;
		SceneViewPanel sceneViewPanel;
		SceneHierarchyPanel sceneHierarchyPanel;
		StatsPanel statsPanel;

		Ref<Texture> debugComputeTexture;
		Ref<ComputeShader> debugComputeShader;
		uint32_t width = 128, height = 128;

	public:
		EditorLayer() : Layer("Editor"), dockSpace(), gameViewPanel(), profilerPanel(),
			sceneViewPanel(), sceneHierarchyPanel(), statsPanel() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		void CopyRuntime(std::istream& binaryStream, size_t binaryLength);
		void UpdateRuntimeLibrary();

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);
	};
}