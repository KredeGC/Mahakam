#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class ComputeShader;

	extern template class Asset<ComputeShader>;

	class ParticleRenderPass : public RenderPass
	{
	private:
		Ref<FrameBuffer> viewportFramebuffer = nullptr;
		Ref<ComputeShader> particleCompute = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~ParticleRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer> src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() override { return viewportFramebuffer; };
	};
}