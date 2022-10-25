#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class Shader;

	class TonemappingRenderPass : public RenderPass
	{
	private:
		Ref<FrameBuffer> viewportFramebuffer = nullptr;
		Ref<Shader> tonemappingShader = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~TonemappingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer> src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() override { return viewportFramebuffer; };
	};
}
