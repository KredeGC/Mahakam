#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class GameViewPanel
	{
	private:
		bool open = true;
		bool focused = false;
		bool hovered = false;

		Ref<Scene> activeScene;
		Ref<Texture> viewportTexture;

		glm::vec2 viewportSize = { 0.0f, 0.0f };

	public:
		GameViewPanel() = default;

		void SetScene(Ref<Scene> scene);

		void SetFrameBuffer(Ref<Texture> tex);

		void OnImGuiRender();

		bool OnMouseScrolled(MouseScrolledEvent& event);
	};
}