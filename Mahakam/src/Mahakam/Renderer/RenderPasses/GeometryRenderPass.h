#pragma once

#include "RenderPass.h"

namespace Mahakam
{
	class GeometryRenderPass : public RenderPass
	{
	protected:
		Asset<FrameBuffer> gBuffer = nullptr;

	public:
		virtual bool Init(uint32_t width, uint32_t height) override;
		virtual ~GeometryRenderPass() override;

		virtual void OnWindowResize(uint32_t width, uint32_t height) override;

		virtual bool Render(SceneData* sceneData, Asset<FrameBuffer>& src) override;

		virtual Asset<FrameBuffer> GetFrameBuffer() { return gBuffer; }
	};
}