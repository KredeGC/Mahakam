#pragma once

#include "Mahakam/Renderer/FrameBuffer.h"

namespace Mahakam
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	private:
		uint32_t rendererID;
		FrameBufferProps prop;
		std::vector<uint32_t> colorAttachments;
		uint32_t depthAttachment = 0;

	public:
		OpenGLFrameBuffer(const FrameBufferProps& prop);
		virtual ~OpenGLFrameBuffer() override;

		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(uint32_t width, uint32_t height) override;

		virtual void attachColorTexture(uint32_t width, uint32_t height, TextureFormat format) override;

		virtual const std::vector<uint32_t> getColorAttachments() const override { return colorAttachments; }
		virtual uint32_t getDepthAttachment() const override { return depthAttachment; }

		virtual const FrameBufferProps& getSpecification() const override { return prop; }

		void invalidate();
	};
}