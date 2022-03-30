#pragma once

#include "RenderPass.h"

#include "Mahakam/Renderer/ComputeShader.h"

namespace Mahakam
{
	class ParticleRenderPass : public RenderPass
	{
	private:
		Ref<FrameBuffer> viewportFramebuffer = nullptr;
		Ref<ComputeShader> particleCompute = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~ParticleRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer>& src) override;

		virtual Ref<FrameBuffer> GetFrameBuffer() { return viewportFramebuffer; }
	};
}