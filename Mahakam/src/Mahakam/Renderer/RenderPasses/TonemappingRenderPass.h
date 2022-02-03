#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class TonemappingRenderPass : public RenderPass
	{
	private:
		Ref<FrameBuffer> viewportFramebuffer = nullptr;
		Ref<Material> tonemappingMaterial = nullptr;

		bool updateTextures = true;

	public:
		TonemappingRenderPass(uint32_t width, uint32_t height);
		virtual ~TonemappingRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual void Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return viewportFramebuffer; }
	};
}
