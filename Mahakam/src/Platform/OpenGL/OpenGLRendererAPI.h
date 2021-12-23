#pragma once

#include "Mahakam/Renderer/RendererAPI.h"

namespace Mahakam
{
	class OpenGLRendererAPI : public RendererAPI
	{
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;

		virtual const char* getGraphicsVendor() override;

		virtual void setClearColor(const glm::vec4 color) override;
		virtual void clear(bool color, bool depth) override;

		virtual void setFillMode(bool fill) override;

		virtual void drawIndexed(uint32_t count) override;
	};
}