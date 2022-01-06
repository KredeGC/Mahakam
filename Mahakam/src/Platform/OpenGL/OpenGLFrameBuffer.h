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

		virtual void resize(uint32_t width, uint32_t height) override;

		virtual const std::vector<Ref<RenderBuffer>>& getColorAttachments() const override { return colorAttachments; }
		virtual const Ref<RenderBuffer>& getDepthAttachment() const override { return depthAttachment; }

		virtual const FrameBufferProps& getSpecification() const override { return props; }

		void invalidate();
	};
}