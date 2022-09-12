#pragma once

#include "Mahakam/Renderer/RenderPasses/LightingRenderPass.h"

namespace Mahakam
{
	class TexelLightingPass : public LightingRenderPass
	{
	protected:
		virtual void SetupFrameBuffer(uint32_t width, uint32_t height) override;
		virtual void SetupShaders() override;

		virtual void SetupTextures(SceneData* sceneData, Ref<FrameBuffer> src) override;
	};
}