#pragma once

#include "Mahakam/Core/Core.h"
#include "TextureFormats.h"
#include "RenderBuffer.h"
#include "Texture.h"

#include <vector>

namespace Mahakam
{
	struct FrameBufferAttachmentProps
	{
		TextureFormat format = TextureFormat::RGBA8;
		TextureFilter filterMode = TextureFilter::Bilinear;
		bool immutable = false;

		FrameBufferAttachmentProps(TextureFormat format)
			: format(format) {}

		FrameBufferAttachmentProps(TextureFormat format, TextureFilter filterMode, bool immutable = false)
			: format(format), filterMode(filterMode), immutable(immutable) {}
	};

	struct FrameBufferProps
	{
		uint32_t width;
		uint32_t height;
		std::vector<FrameBufferAttachmentProps> colorAttachments;
		FrameBufferAttachmentProps depthAttachment = { TextureFormat::Depth24, TextureFilter::Bilinear, true };

		bool dontUseDepth = false;
		bool swapChainTarget = false;

		FrameBufferProps() = default;

		FrameBufferProps(uint32_t width, uint32_t height, std::initializer_list<FrameBufferAttachmentProps> colorAttachments,
			FrameBufferAttachmentProps depthAttachment = { TextureFormat::Depth24, TextureFilter::Bilinear, true }, bool dontUseDepth = false, bool swapChainTarget = false)
			: width(width), height(height), colorAttachments(colorAttachments), depthAttachment(depthAttachment), dontUseDepth(dontUseDepth), swapChainTarget(swapChainTarget) {}
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void blit(const Ref<FrameBuffer>& dest) = 0;
		virtual void blitDepth(const Ref<FrameBuffer>& dest) = 0;
		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual const std::vector<Ref<RenderBuffer>>& getColorBuffers() const = 0;
		virtual const Ref<RenderBuffer>& getColorBuffer(int index) const = 0;
		virtual const Ref<Texture>& getColorTexture(int index) const = 0;

		virtual const Ref<RenderBuffer>& getDepthBuffer() const = 0;
		virtual const Ref<Texture>& getDepthTexture() const = 0;

		virtual const FrameBufferProps& getSpecification() const = 0;

		virtual void readColorPixels(void* pixels, int attachmentSlot = 0) const = 0;

		static Ref<FrameBuffer> create(const FrameBufferProps& props);
	};
}