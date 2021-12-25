#pragma once

#include "RendererAPI.h"

namespace Mahakam
{
	class GL
	{
	private:
		static RendererAPI* rendererAPI;

	public:
		inline static void init() { rendererAPI->init(); }

		inline static void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) { rendererAPI->setViewport(x, y, w, h); }

		inline static const char* getGraphicsVendor() { return rendererAPI->getGraphicsVendor(); }

		inline static void setClearColor(const glm::vec4& color) { rendererAPI->setClearColor(color); }
		inline static void clear(bool color = true, bool depth = true) { rendererAPI->clear(color, depth); }

		inline static void setFillMode(bool fill) { rendererAPI->setFillMode(fill); }

		inline static void drawIndexed(uint32_t indexCount) { rendererAPI->drawIndexed(indexCount); }

		inline static void drawInstanced(uint32_t indexCount, uint32_t count) { rendererAPI->drawInstanced(indexCount, count); }
	};
}