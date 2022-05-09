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

		Asset<FrameBuffer> viewportFramebuffer;

		Asset<Shader> blitShader;

	public:
		RenderPassPanel();

		virtual void OnImGuiRender() override;

	private:
		void RenderPanel();

		void RenderImage(Asset<FrameBuffer> frameBuffer, int index, bool depth);
	};
}