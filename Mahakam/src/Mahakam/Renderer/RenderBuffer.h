#pragma once

#include "Mahakam/Core/Core.h"

#include "TextureFormats.h"

namespace Mahakam
{
	class RenderBuffer
	{
	public:
		virtual ~RenderBuffer() = default;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getRendererID() const = 0;

		virtual uint32_t getSize() const = 0;

		static Ref<RenderBuffer> create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);
	};
}