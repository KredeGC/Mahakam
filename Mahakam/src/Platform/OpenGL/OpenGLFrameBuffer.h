#pragma once

#include "Mahakam/Renderer/FrameBuffer.h"

namespace Mahakam
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	private:
		uint32_t rendererID;
		FrameBufferProps props;
		std::vector<Ref<RenderBuffer>> colorAttachments;
		Ref<RenderBuffer> depthAttachment = 0;

	public:
		OpenGLFrameBuffer(const FrameBufferProps& props);
		virtual ~OpenGLFrameBuffer() override;

		virtual void bind() override;
		virtual void unbind() override;

		virtual void blit(const Ref<FrameBuffer>& dest) override;
		virtual void blitDepth(const Ref<FrameBuffer>& dest) override;
		virtual void resize(uint32_t width, uint32_t height) override;

		virtual const std::vector<Ref<RenderBuffer>>& getColorBuffers() const override { return colorAttachments; }
		virtual Ref<RenderBuffer> getColorBuffer(int index) const override { return colorAttachments[index]; }
		virtual Ref<Texture> getColorTexture(int index) const override { return std::static_pointer_cast<Texture>(colorAttachments[index]); }

		virtual Ref<RenderBuffer> getDepthBuffer() const override { return depthAttachment; }
		virtual Ref<Texture> getDepthTexture() const override { return std::static_pointer_cast<Texture>(depthAttachment); }

		virtual const FrameBufferProps& getSpecification() const override { return props; }

		virtual void readColorPixels(void* pixels, int attachmentSlot) const;

		void invalidate();
	};
}