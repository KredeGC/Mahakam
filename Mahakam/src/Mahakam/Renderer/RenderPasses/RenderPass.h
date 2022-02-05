#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Renderer/FrameBuffer.h"

#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;

		virtual bool Render(Renderer::SceneData* sceneData, Ref<FrameBuffer>& src) = 0;

		virtual Ref<FrameBuffer> GetFrameBuffer() = 0;
	};
}