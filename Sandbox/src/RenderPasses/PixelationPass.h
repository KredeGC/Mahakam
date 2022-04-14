#pragma once
#include "Mahakam/Renderer/RenderPasses/RenderPass.h"

namespace Mahakam
{
	class PixelationPass : public RenderPass
	{
	protected:
		Ref<FrameBuffer> viewportFramebuffer = nullptr;
		Ref<Shader> pixelationShader = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~PixelationPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return viewportFramebuffer; }
	};
}