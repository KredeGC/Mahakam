#pragma once

#include "Mahakam/Renderer/FrameBuffer.h"

namespace Mahakam
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	private:
		uint32_t rendererID = 0;
		FrameBufferProps props;
		std::vector<Asset<RenderBuffer>> colorAttachments;
		Asset<RenderBuffer> depthAttachment = nullptr;

	public:
		OpenGLFrameBuffer(const FrameBufferProps& props);
		virtual ~OpenGLFrameBuffer() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Blit(Asset<FrameBuffer> dest, bool color = true, bool depth = true) override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual const std::vector<Asset<RenderBuffer>>& GetColorBuffers() const override { return colorAttachments; }
		virtual Asset<RenderBuffer> GetColorBuffer(int index) const override { return colorAttachments[index]; }
		virtual Asset<Texture> GetColorTexture(int index) const override { return colorAttachments[index]; }

		virtual Asset<RenderBuffer> GetDepthBuffer() const override { return depthAttachment; }
		virtual Asset<Texture> GetDepthTexture() const override { return depthAttachment; }

		virtual const FrameBufferProps& GetSpecification() const override { return props; }

		virtual void ReadColorPixels(void* pixels, int attachmentSlot) const;

		void Invalidate();
	};
}