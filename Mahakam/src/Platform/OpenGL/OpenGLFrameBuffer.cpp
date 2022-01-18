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

	void OpenGLFrameBuffer::blit(const Ref<FrameBuffer>& dest)
	{
		Ref<OpenGLFrameBuffer> fbo = std::static_pointer_cast<OpenGLFrameBuffer>(dest);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, rendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->rendererID);

		glBlitFramebuffer(0, 0, props.width, props.height, 0, 0, fbo->props.width, fbo->props.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBlitFramebuffer(0, 0, props.width, props.height, 0, 0, fbo->props.width, fbo->props.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::blitDepth(const Ref<FrameBuffer>& dest)
	{
		Ref<OpenGLFrameBuffer> fbo = std::static_pointer_cast<OpenGLFrameBuffer>(dest);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, rendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->rendererID);

		glBlitFramebuffer(0, 0, props.width, props.height, 0, 0, fbo->props.width, fbo->props.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

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

	void OpenGLFrameBuffer::readColorPixels(void* pixels, int attachmentSlot) const
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(attachmentSlot < colorAttachments.size(), "Index outside range of framebuffer textures!");

		auto& spec = props.colorAttachments[attachmentSlot];

		GLenum format = TextureFormatToOpenGLFormat(spec.format);
		GLenum type = TextureFormatToOpenGLType(spec.format);

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentSlot);

		glReadPixels(0, 0, props.width, props.height, format, type, pixels);
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

		// Color buffers
		for (int i = 0; i < props.colorAttachments.size(); i++)
		{
			FrameBufferAttachmentProps& spec = props.colorAttachments[i];

			Ref<Texture> tex = Texture2D::create({ props.width, props.height, spec.format, spec.filterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });

			colorAttachments.push_back(tex);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex->getRendererID(), 0);
		}

		// Depth buffer
		if (!props.dontUseDepth)
		{
			FrameBufferAttachmentProps& spec = props.depthAttachment;

			uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(spec.format);
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

		if (colorAttachments.size() > 1)
		{
			GLenum* buffers = new GLenum[colorAttachments.size()];
			for (GLenum i = 0; i < colorAttachments.size(); i++)
				buffers[i] = GL_COLOR_ATTACHMENT0 + i;

			glDrawBuffers(colorAttachments.size(), buffers);

			delete[] buffers;
		}
		else if (colorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		// Check framebuffer
		MH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}