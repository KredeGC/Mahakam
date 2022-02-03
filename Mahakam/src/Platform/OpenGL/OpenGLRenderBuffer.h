#pragma once
#include "Mahakam/Renderer/RenderBuffer.h"

namespace Mahakam
{
	class OpenGLRenderBuffer : public RenderBuffer
	{
	private:
		uint32_t rendererID;
		uint32_t width;
		uint32_t height;

		uint32_t internalFormat;

		uint32_t size;

	public:
		OpenGLRenderBuffer(uint32_t width, uint32_t height, TextureFormat format);
		virtual ~OpenGLRenderBuffer() override;

		virtual uint32_t GetWidth() const { return width; }
		virtual uint32_t GetHeight() const { return height; }
		virtual uint32_t GetRendererID() const { return rendererID; }

		virtual uint32_t GetSize() const override { return size; }
	};
}