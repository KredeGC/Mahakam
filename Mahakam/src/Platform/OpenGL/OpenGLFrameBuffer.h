#pragma once

#include "Mahakam/Renderer/FrameBuffer.h"

namespace Mahakam
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	private:
		uint32_t m_RendererID = 0;
		FrameBufferProps m_Props;
		std::vector<Asset<RenderBuffer>> m_ColorAttachments;
		Asset<RenderBuffer> m_DepthAttachment = nullptr;

		inline static constexpr uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

	public:
		OpenGLFrameBuffer(const FrameBufferProps& props);
		virtual ~OpenGLFrameBuffer() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Blit(Asset<FrameBuffer> dest, bool color = true, bool depth = true) override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual const std::vector<Asset<RenderBuffer>>& GetColorBuffers() const override { return m_ColorAttachments; }
		virtual Asset<RenderBuffer> GetColorBuffer(int index) const override { return m_ColorAttachments[index]; }
		virtual Asset<Texture> GetColorTexture(int index) const override;

		virtual Asset<RenderBuffer> GetDepthBuffer() const override { return m_DepthAttachment; }
		virtual Asset<Texture> GetDepthTexture() const override;

		virtual const FrameBufferProps& GetSpecification() const override { return m_Props; }

		virtual void ReadColorPixels(void* pixels, int attachmentSlot) const override;

		void Invalidate();
	};
}