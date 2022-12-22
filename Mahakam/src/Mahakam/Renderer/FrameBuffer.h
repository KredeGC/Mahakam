#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Types.h"

#include "Mahakam/Asset/Asset.h"

#include "TextureFormats.h"

namespace Mahakam
{
	class RenderBuffer;
	class Texture;

	struct FrameBufferAttachmentProps
	{
		TextureFormat Format;
		TextureFilter FilterMode;
		bool Immutable;
        
        FrameBufferAttachmentProps() :
            Format(TextureFormat::RGBA8), FilterMode(TextureFilter::Bilinear), Immutable(false) {}

		FrameBufferAttachmentProps(TextureFormat format)
			: Format(format), FilterMode(TextureFilter::Bilinear), Immutable(false)  {}

		FrameBufferAttachmentProps(TextureFormat format, TextureFilter filterMode, bool immutable = false)
			: Format(format), FilterMode(filterMode), Immutable(immutable) {}
	};

	struct FrameBufferProps
	{
		uint32_t Width;
		uint32_t Height;
		TrivialVector<FrameBufferAttachmentProps> ColorAttachments;
		FrameBufferAttachmentProps DepthAttachment = { TextureFormat::Depth24, TextureFilter::Bilinear, true };

		bool DontUseDepth = false;
		bool SwapChainTarget = false;

		FrameBufferProps() = default;

		FrameBufferProps(uint32_t width, uint32_t height, std::initializer_list<FrameBufferAttachmentProps> colorAttachments,
			FrameBufferAttachmentProps depthAttachment = { TextureFormat::Depth24, TextureFilter::Bilinear, true }, bool dontUseDepth = false, bool swapChainTarget = false)
			: Width(width), Height(height), ColorAttachments(colorAttachments), DepthAttachment(depthAttachment), DontUseDepth(dontUseDepth), SwapChainTarget(swapChainTarget) {}
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