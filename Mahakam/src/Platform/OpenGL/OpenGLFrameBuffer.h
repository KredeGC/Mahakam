#pragma once

#include "Mahakam/Renderer/FrameBuffer.h"

namespace Mahakam
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	private:
		uint32_t rendererID = 0;
		FrameBufferProps props;
		std::vector<Ref<RenderBuffer>> colorAttachments;
		Ref<RenderBuffer> depthAttachment = 0;

	public:
		OpenGLFrameBuffer(const FrameBufferProps& props);
		virtual ~OpenGLFrameBuffer() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Blit(Ref<FrameBuffer> dest, bool color = true, bool depth = true) override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual const std::vector<Ref<RenderBuffer>>& GetColorBuffers() const override { return colorAttachments; }
		virtual Ref<RenderBuffer> GetColorBuffer(int index) const override { return colorAttachments[index]; }
		virtual Ref<Texture> GetColorTexture(int index) const override { return std::static_pointer_cast<Texture>(colorAttachments[index]); }

		virtual Ref<RenderBuffer> GetDepthBuffer() const override { return depthAttachment; }
		virtual Ref<Texture> GetDepthTexture() const override { return std::static_pointer_cast<Texture>(depthAttachment); }

		virtual const FrameBufferProps& GetSpecification() const override { return props; }

		virtual void ReadColorPixels(void* pixels, int attachmentSlot) const;

		void Invalidate();
	};
}