#pragma once

#include "EditorCamera.h"

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

		EditorCamera editorCamera;
		Ref<Texture> viewportTexture;

		glm::vec2 viewportSize = { 0.0f, 0.0f };

	public:
		SceneViewPanel() = default;

		void OnUpdate(Timestep dt);

		void OnImGuiRender();

		void OnEvent(Event& event);

		void SetFrameBuffer(Ref<Texture> tex) { viewportTexture = tex; }

		EditorCamera& GetCamera() { return editorCamera; }
	};
}