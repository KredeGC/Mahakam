#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class Shader;

	extern template class Asset<Shader>;

	class TonemappingRenderPass : public RenderPass
	{
	private:
		Asset<FrameBuffer> viewportFramebuffer = nullptr;
		Asset<Shader> tonemappingShader = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~TonemappingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Asset<FrameBuffer> src) override;

		virtual Asset<FrameBuffer> GetFrameBuffer() override { return viewportFramebuffer; };
	};
}
