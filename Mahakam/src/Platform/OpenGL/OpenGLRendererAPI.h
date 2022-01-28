#pragma once

#include "Mahakam/Renderer/RendererAPI.h"

namespace Mahakam
{
	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void init() override;
		virtual const char* getGraphicsVendor() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual void finishRendering() override;

		virtual void setClearColor(const glm::vec4 color) override;
		virtual void clear(bool color, bool depth) override;

		virtual void enableCulling(bool enable, bool cullFront) override;

		virtual void enableZWriting(bool enable) override;

		virtual void enableZTesting(bool enable) override;

		virtual void setFillMode(bool fill) override;

		virtual void setBlendMode(BlendMode src, BlendMode dst, bool enable) override;

		virtual void drawIndexed(uint32_t count) override;

		virtual void drawInstanced(uint32_t indexCount, uint32_t count) override;
	};
}