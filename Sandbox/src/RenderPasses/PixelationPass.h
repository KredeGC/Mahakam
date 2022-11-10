#pragma once
#include "Mahakam/Renderer/RenderPasses/RenderPass.h"

namespace Mahakam
{
	class PixelationPass : public RenderPass
	{
	protected:
		Asset<FrameBuffer> viewportFramebuffer = nullptr;
		Asset<Shader> pixelationShader = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~PixelationPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Asset<FrameBuffer> src) override;

		virtual Asset<FrameBuffer> GetFrameBuffer() override { return viewportFramebuffer; }
	};
}