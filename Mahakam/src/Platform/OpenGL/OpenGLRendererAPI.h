#pragma once

#include "Mahakam/Renderer/RendererAPI.h"

namespace Mahakam
{
	class OpenGLRendererAPI : public RendererAPI
	{
	private:
		bool m_ScissorEnabled = false;

	public:
		virtual void Init() override;
		virtual const char* GetGraphicsVendor() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor) override;

		virtual void FinishRendering() override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear(bool color, bool depth) override;

		virtual void EnableCulling(bool enable, bool cullFront) override;
		virtual void EnableZWriting(bool enable) override;

		virtual void SetZTesting(DepthMode mode) override;
		virtual void SetFillMode(bool fill) override;
		virtual void SetBlendMode(BlendMode src, BlendMode dst, bool enable) override;

		virtual void DrawIndexed(uint32_t count) override;
		virtual void DrawInstanced(uint32_t indexCount, uint32_t count) override;
	};
}