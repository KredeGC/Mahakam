#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class Shader;

	class TonemappingRenderPass : public RenderPass
	{
	private:
		Asset<FrameBuffer> m_ViewportFramebuffer = nullptr;
		Asset<Shader> m_TonemappingShader = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~TonemappingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, const Asset<FrameBuffer>& src) override;

		virtual Asset<FrameBuffer> GetFrameBuffer() override { return m_ViewportFramebuffer; };
	};
}
