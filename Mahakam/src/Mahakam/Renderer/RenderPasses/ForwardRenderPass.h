#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class ForwardRenderPass : public RenderPass
	{
	private:
		Ref<FrameBuffer> forwardFramebuffer = nullptr;

	public:
		ForwardRenderPass(uint32_t width, uint32_t height);
		virtual ~ForwardRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual void Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return forwardFramebuffer; }
	};
}