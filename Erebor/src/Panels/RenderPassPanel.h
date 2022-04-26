#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class RenderPassPanel : EditorWindow
	{
	private:
		bool open = true;
		int frameBufferIndex = 0;
		int textureIndex = 0;

		Ref<FrameBuffer> viewportFramebuffer;

		Ref<Shader> blitShader;

	public:
		RenderPassPanel();

		virtual void OnImGuiRender() override;

	private:
		void RenderPanel();

		void RenderImage(Ref<FrameBuffer> frameBuffer, int index, bool depth);
	};
}