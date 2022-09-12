#pragma once

#include <Mahakam/Mahakam.h>

#include <imgui/imgui.h>

namespace Mahakam::Editor
{
	class RenderPassPanel : EditorWindow
	{
	private:
		bool m_Open = true;
		int m_FrameBufferIndex = 0;
		int m_TextureIndex = 0;

		Ref<FrameBuffer> m_ViewportFramebuffer;

		Asset<Shader> m_BlitShader;

	public:
		RenderPassPanel();

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

	private:
		void RenderPanel();

		void RenderImage(Ref<FrameBuffer> frameBuffer, int index, bool depth);
	};
}