#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class RenderPassPanel : EditorWindow
	{
	private:
		bool m_Open = true;
		int m_FrameBufferIndex = 0;
		int m_TextureIndex = 0;

		Asset<FrameBuffer> m_ViewportFramebuffer;

		Asset<Shader> m_BlitShader;

	public:
		RenderPassPanel();

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

	private:
		void RenderPanel();

		void RenderImage(Asset<FrameBuffer> frameBuffer, int index, bool depth);
	};
}