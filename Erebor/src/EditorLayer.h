#pragma once

#ifndef MH_RUNTIME
#include "Panels/DockSpace.h"
#include "Panels/GameViewPanel.h"
#include "Panels/ProfilerPanel.h"
#include "Panels/RenderPassPanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/SceneViewPanel.h"
#include "Panels/StatsPanel.h"
#include "EditorCamera.h"
#endif

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class EditorLayer : public Layer
	{
	private:
		inline static Entity s_SelectedEntity;

		inline static Ref<Scene> s_ActiveScene;

#ifndef MH_RUNTIME
		DockSpace m_DockSpace;
#endif

	public:
#ifndef MH_RUNTIME
		EditorLayer() : Layer("Editor"), m_DockSpace() {}
#else
		EditorLayer() : Layer("Editor") {}
#endif

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

		inline static void SetSelectedEntity(Entity entity) { s_SelectedEntity = entity; }
		inline static Entity GetSelectedEntity() { return s_SelectedEntity; }

		inline static void SetActiveScene(Ref<Scene> scene) { s_ActiveScene = scene; }
		inline static Ref<Scene> GetActiveScene() { return s_ActiveScene; }

	private:
		void CopyRuntime(std::istream& binaryStream, size_t binaryLength);
		void UpdateRuntimeLibrary();

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnWindowResized(WindowResizeEvent& event);
	};
}