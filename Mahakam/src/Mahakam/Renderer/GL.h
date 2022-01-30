#pragma once

#include "RendererAPI.h"

#include "Mesh.h"

namespace Mahakam
{
	class GL
	{
	private:
		static RendererAPI* rendererAPI;

	public:
		static void init();

		inline static const char* getGraphicsVendor() { return rendererAPI->getGraphicsVendor(); }

		inline static void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) { rendererAPI->setViewport(x, y, w, h); }

		inline static void finishRendering() { rendererAPI->finishRendering(); }

		inline static void setClearColor(const glm::vec4& color) { rendererAPI->setClearColor(color); }
		inline static void clear(bool color = true, bool depth = true) { rendererAPI->clear(color, depth); }

		inline static void enableCulling(bool enable, bool cullFront = false) { rendererAPI->enableCulling(enable, cullFront); }

		inline static void enableZWriting(bool enable) { rendererAPI->enableZWriting(enable); }

		inline static void enableZTesting(bool enable) { rendererAPI->enableZTesting(enable); }

		inline static void setFillMode(bool fill) { rendererAPI->setFillMode(fill); }

		inline static void setBlendMode(RendererAPI::BlendMode src, RendererAPI::BlendMode dst, bool enable) { rendererAPI->setBlendMode(src, dst, enable); }

		inline static void drawScreenQuad() { Mesh::getScreenQuad()->bind(); drawIndexed(Mesh::getScreenQuad()->getIndexCount()); }

		inline static void drawIndexed(uint32_t indexCount) { rendererAPI->drawIndexed(indexCount); }

		inline static void drawInstanced(uint32_t indexCount, uint32_t count) { rendererAPI->drawInstanced(indexCount, count); }
	};
}