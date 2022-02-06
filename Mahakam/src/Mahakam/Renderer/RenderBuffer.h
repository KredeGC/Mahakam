#pragma once

#include "Mahakam/Core/Core.h"

#include "TextureFormats.h"

namespace Mahakam
{
	class RenderBuffer
	{
	public:
		virtual ~RenderBuffer() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual uint32_t GetSize() const = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		static Ref<RenderBuffer> Create(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);
	};
}