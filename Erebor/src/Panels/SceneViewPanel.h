#pragma once

#include "EditorCamera.h"

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class SceneViewPanel
	{
	private:
		bool m_Open = true;
		bool m_Focused = false;
		bool m_Hovered = false;

		EditorCamera m_EditorCamera;
		Ref<Texture> m_ViewportTexture;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

	public:
		SceneViewPanel() = default;

		void OnUpdate(Timestep dt);

		void OnImGuiRender();

		void OnEvent(Event& event);
		bool OnKeyPressed(KeyPressedEvent& event);

		void SetFrameBuffer(Ref<Texture> tex) { m_ViewportTexture = tex; }

		EditorCamera& GetCamera() { return m_EditorCamera; }
	};
}