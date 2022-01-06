#pragma once

#include "Mahakam/Core/Core.h"
#include "TextureFormats.h"
#include "RenderBuffer.h"

#include <vector>

namespace Mahakam
{
	struct FrameBufferAttachmentProps
	{
		TextureFormat format = TextureFormat::RGBA8;
		TextureFilter filterMode = TextureFilter::Bilinear;
		bool immutable = false;
	};

	struct FrameBufferProps
	{
		uint32_t width;
		uint32_t height;
		std::vector<FrameBufferAttachmentProps> colorAttachments;
		FrameBufferAttachmentProps depthAttachment = { TextureFormat::Depth24, TextureFilter::Bilinear, true };

		bool swapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual const std::vector<Ref<RenderBuffer>>& getColorAttachments() const = 0;
		virtual const Ref<RenderBuffer>& getDepthAttachment() const = 0;

		virtual const FrameBufferProps& getSpecification() const = 0;

		static Ref<FrameBuffer> create(const FrameBufferProps& props);
	};
}