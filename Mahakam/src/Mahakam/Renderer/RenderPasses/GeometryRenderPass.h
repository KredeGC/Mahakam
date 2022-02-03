#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class GeometryRenderPass : public RenderPass
	{
	private:
		Ref<FrameBuffer> gBuffer = nullptr;

	public:
		GeometryRenderPass(uint32_t width, uint32_t height);
		virtual ~GeometryRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual void Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return gBuffer; }
	};
}