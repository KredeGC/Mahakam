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

	public:
		OpenGLRenderBuffer(uint32_t width, uint32_t height, TextureFormat format);
		virtual ~OpenGLRenderBuffer() override;

		virtual uint32_t getWidth() const { return width; }
		virtual uint32_t getHeight() const { return height; }
		virtual uint32_t getRendererID() const { return rendererID; }
	};
}