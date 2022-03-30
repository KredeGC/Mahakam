#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Renderer/Shader.h"

#include "Mahakam/Renderer/FrameBuffer.h"

#include "Mahakam/Renderer/RenderData.h"

namespace Mahakam
{
	class RenderPass
	{
	private:
		bool m_Initialized = false;

	public:
		inline virtual bool Init(uint32_t width, uint32_t height)
		{
			if (m_Initialized)
				return true;

			m_Initialized = true;

			return false;
		}

		virtual ~RenderPass() = default;

		virtual void OnWindowResize(uint32_t width, uint32_t height) {}

		virtual bool Render(SceneData* sceneData, Ref<FrameBuffer>& src) = 0;

		virtual Ref<FrameBuffer> GetFrameBuffer() = 0;

		bool IsInitialized() const { return m_Initialized; }
	};
}