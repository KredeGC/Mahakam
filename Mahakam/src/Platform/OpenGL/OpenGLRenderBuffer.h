#pragma once

#include "Mahakam/Renderer/RenderBuffer.h"

namespace Mahakam
{
	class OpenGLRenderBuffer : public RenderBuffer
	{
	private:
		uint32_t m_RendererID;
		uint32_t m_Width;
		uint32_t m_Height;

		uint32_t m_InternalFormat;

		uint32_t m_Size;

	public:
		OpenGLRenderBuffer(uint32_t width, uint32_t height, TextureFormat format);

		OpenGLRenderBuffer(const OpenGLRenderBuffer& other);
		OpenGLRenderBuffer(OpenGLRenderBuffer&& other) noexcept;

		virtual ~OpenGLRenderBuffer() override;

		OpenGLRenderBuffer& operator=(const OpenGLRenderBuffer& rhs);
		OpenGLRenderBuffer& operator=(OpenGLRenderBuffer&& rhs) noexcept;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual uint32_t GetSize() const override { return m_Size; }

		virtual void Resize(uint32_t width, uint32_t height) override;
	};
}