#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class RenderPassPanel
	{
	private:
		bool open = true;
		int frameBufferIndex = 0;
		int textureIndex = 0;

		Ref<FrameBuffer> viewportFramebuffer;

		Ref<Shader> blitShader;

	public:
		RenderPassPanel();

		void OnImGuiRender();

	private:
		void RenderPanel();

		void RenderImage(Ref<FrameBuffer> frameBuffer, int index, bool depth);
	};
}