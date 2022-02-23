#pragma once
#include "Mahakam/Renderer/RenderPasses/GeometryRenderPass.h"

namespace Mahakam
{
	class TexelGeometryPass : public GeometryRenderPass
	{
	public:
		virtual void Init(uint32_t width, uint32_t height) override;
	};
}