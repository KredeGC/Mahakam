#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class SceneViewPanel
	{
	private:
		bool open = true;
		bool focused = false;
		bool hovered = false;

		Ref<Scene> activeScene;
		Ref<FrameBuffer> viewportFramebuffer;

		glm::vec2 viewportSize = { 0.0f, 0.0f };

	public:
		SceneViewPanel() = default;

		void SetScene(Ref<Scene> scene);

		void SetFrameBuffer(Ref<FrameBuffer> framebuffer);

		void OnImGuiRender();

		bool OnMouseScrolled(MouseScrolledEvent& event);
	};
}