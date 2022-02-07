#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Renderer/Shader.h"

#include "Mahakam/Renderer/FrameBuffer.h"

#include "Mahakam/Renderer/RenderData.h"

namespace Mahakam
{
	class RenderPass
	{
	public:
		virtual void Init(uint32_t width, uint32_t height) = 0;
		virtual ~RenderPass() = default;

		virtual void OnWindowResize(uint32_t width, uint32_t height) {}

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer>& src) = 0;

		virtual Ref<FrameBuffer> GetFrameBuffer() = 0;
	};
}