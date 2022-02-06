#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class TonemappingRenderPass : public RenderPass
	{
	private:
		Ref<FrameBuffer> viewportFramebuffer = nullptr;
		Ref<Shader> tonemappingShader = nullptr;

	public:
		TonemappingRenderPass(uint32_t width, uint32_t height);
		virtual ~TonemappingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return viewportFramebuffer; }
	};
}
