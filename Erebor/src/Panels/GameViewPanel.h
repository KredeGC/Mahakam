#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class GameViewPanel : EditorWindow
	{
	private:
		bool m_Open = true;
		bool m_Focused = false;
		bool m_Hovered = false;

		WeakRef<Scene> m_ActiveScene;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

	public:
		GameViewPanel() = default;

		virtual void OnImGuiRender() override;

		bool OnMouseScrolled(MouseScrolledEvent& event);
	};
}