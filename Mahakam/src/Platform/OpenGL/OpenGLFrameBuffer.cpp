#include "mhpch.h"
#include "OpenGLFrameBuffer.h"

#include "OpenGLTextureFormats.h"

#include <glad/glad.h>

namespace Mahakam
{
	static const uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferProps& props)
		: props(props)
	{
		invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		MH_PROFILE_FUNCTION();

		glDeleteFramebuffers(1, &rendererID);
	}

	void OpenGLFrameBuffer::bind()
	{
		MH_PROFILE_FUNCTION();

		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
		glViewport(0, 0, props.width, props.height);
	}

	void OpenGLFrameBuffer::unbind()
	{
		MH_PROFILE_FUNCTION();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			MH_CORE_WARN("Attempted to resize framebuffer to an unsupported size: ({0},{1})", width, height);
			return;
		}

		props.width = width;
		props.height = height;

		invalidate();
	}

	void OpenGLFrameBuffer::invalidate()
	{
		MH_PROFILE_FUNCTION();

		if (rendererID)
		{
			glDeleteFramebuffers(1, &rendererID);

			colorAttachments.clear();
			depthAttachment = 0;
		}

		glCreateFramebuffers(1, &rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

		// Color buffer
		for (int i = 0; i < props.colorAttachments.size(); i++)
		{
			FrameBufferAttachmentProps& spec = props.colorAttachments[i];

			Ref<Texture> tex = Texture2D::create({ props.width, props.height, spec.format, spec.filterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });

			colorAttachments.push_back(tex);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getRendererID(), 0);
		}

		// Depth buffer
		{
			FrameBufferAttachmentProps& spec = props.depthAttachment;

			uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(spec.format);
			uint32_t dataFormat = TextureFormatToOpenGLFormat(spec.format);
			uint32_t type = TextureFormatToOpenGLType(spec.format);
			uint32_t attachment = TextureFormatToOpenGLAttachment(spec.format);

			if (spec.immutable)
			{
				depthAttachment = RenderBuffer::create(props.width, props.height, spec.format);

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, depthAttachment->getRendererID());
			}
			else
			{
				depthAttachment = Texture2D::create({ props.width, props.height, spec.format, spec.filterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });

				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, depthAttachment->getRendererID(), 0);
			}
		}

		// Bind framebuffer
		MH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}