#pragma once
#include "VertexArray.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		enum class BlendMode {
			Zero = 0,
			One,
			SrcColor,
			SrcAlpha,
			OneMinusSrcColor,
			OneMinusSrcAlpha,
			DstColor,
			DstAlpha
		};
	private:
		static API api;

	public:
		virtual void init() = 0;
		virtual const char* getGraphicsVendor() = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

		virtual void finishRendering() = 0;

		virtual void setClearColor(const glm::vec4 color) = 0;
		virtual void clear(bool color, bool depth) = 0;

		virtual void enableCulling(bool enable, bool cullFront) = 0;

		virtual void enableZWriting(bool enable) = 0;

		virtual void enableZTesting(bool enable) = 0;

		virtual void setFillMode(bool fill) = 0;

		virtual void setBlendMode(BlendMode src, BlendMode dst, bool enable) = 0;

		virtual void drawIndexed(uint32_t count) = 0;

		virtual void drawInstanced(uint32_t indexCount, uint32_t count) = 0;

		inline static API getAPI() { return api; }
	};
}