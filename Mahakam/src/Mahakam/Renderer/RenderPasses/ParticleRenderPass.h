#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class ComputeShader;

	class ParticleRenderPass : public RenderPass
	{
	private:
		Asset<FrameBuffer> viewportFramebuffer = nullptr;
		Asset<ComputeShader> particleCompute = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~ParticleRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Asset<FrameBuffer> src) override;

		virtual Asset<FrameBuffer> GetFrameBuffer() { return viewportFramebuffer; }
	};
}