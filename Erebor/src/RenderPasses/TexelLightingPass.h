#pragma once

#include "Mahakam/Renderer/RenderPasses/LightingRenderPass.h"

namespace Mahakam
{
	class TexelLightingPass : public LightingRenderPass
	{
	public:
		virtual void Init(uint32_t width, uint32_t height) override;

	protected:
		virtual void SetupTextures(SceneData* sceneData, Ref<FrameBuffer> src) override;
	};
}