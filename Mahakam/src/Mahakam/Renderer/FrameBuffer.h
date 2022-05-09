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

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Blit(Asset<FrameBuffer> dest, bool color = true, bool depth = true) = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual const std::vector<Asset<RenderBuffer>>& GetColorBuffers() const = 0;
		virtual Asset<RenderBuffer> GetColorBuffer(int index) const = 0;
		virtual Asset<Texture> GetColorTexture(int index) const = 0;

		virtual Asset<RenderBuffer> GetDepthBuffer() const = 0;
		virtual Asset<Texture> GetDepthTexture() const = 0;

		virtual const FrameBufferProps& GetSpecification() const = 0;

		virtual void ReadColorPixels(void* pixels, int attachmentSlot = 0) const = 0;

		MH_DECLARE_FUNC(Create, Asset<FrameBuffer>, const FrameBufferProps& props);
	};
}