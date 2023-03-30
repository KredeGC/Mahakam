#pragma once

#include "Mahakam/Renderer/RenderBuffer.h"

namespace Mahakam
{
	class HeadlessRenderBuffer : public RenderBuffer
	{
	private:
		uint32_t m_Width;
		uint32_t m_Height;

		TextureFormat m_Format;

		uint32_t m_Size;

	public:
		HeadlessRenderBuffer(uint32_t width, uint32_t height, TextureFormat format);

		HeadlessRenderBuffer(const HeadlessRenderBuffer&) = default;
		HeadlessRenderBuffer(HeadlessRenderBuffer&&) noexcept = default;

		virtual ~HeadlessRenderBuffer() override = default;

		HeadlessRenderBuffer& operator=(const HeadlessRenderBuffer&) = default;
		HeadlessRenderBuffer& operator=(HeadlessRenderBuffer&&) noexcept = default;

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return 0; }

		virtual uint32_t GetSize() const override { return m_Size; }

		virtual void Resize(uint32_t width, uint32_t height) override;
	};
}