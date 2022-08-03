#pragma once

#include "EditorCamera.h"

#include <Mahakam.h>

namespace Mahakam::Editor
{
	class SceneViewPanel : EditorWindow
	{
	private:
		bool m_Open = true;
		bool m_Focused = false;
		bool m_Hovered = false;

		EditorCamera m_EditorCamera;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;
		int m_LocalScope = 0;

	public:
		SceneViewPanel() = default;

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnUpdate(Timestep dt) override;

		virtual void OnImGuiRender() override;

		bool OnEvent(Event& event);
		bool OnKeyPressed(KeyPressedEvent& event);

		EditorCamera& GetCamera() { return m_EditorCamera; }
	};
}