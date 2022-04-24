#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class GameViewPanel
	{
	private:
		bool m_Open = true;
		bool m_Focused = false;
		bool m_Hovered = false;

		WeakRef<Scene> m_ActiveScene;
		Ref<Texture> m_ViewportTexture;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

	public:
		GameViewPanel() = default;

		void SetFrameBuffer(Ref<Texture> tex);

		void OnImGuiRender();

		bool OnMouseScrolled(MouseScrolledEvent& event);
	};
}