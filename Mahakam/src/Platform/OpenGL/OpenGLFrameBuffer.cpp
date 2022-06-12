#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLFrameBuffer.h"

#include "OpenGLTextureFormats.h"

#include "Mahakam/Renderer/RenderBuffer.h"
#include "Mahakam/Renderer/Texture.h"

#include <glad/glad.h>

namespace Mahakam
{
	static const uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteFramebuffers(1, &rendererID));
	}

	void OpenGLFrameBuffer::Bind()
	{
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rendererID));
		MH_GL_CALL(glViewport(0, 0, props.width, props.height));
	}

	void OpenGLFrameBuffer::Unbind()
	{
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void OpenGLFrameBuffer::Blit(Asset<FrameBuffer> dest, bool color, bool depth)
	{
		Asset<OpenGLFrameBuffer> fbo = dest;

		MH_GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, rendererID));
		MH_GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->rendererID));

		if (color)
			MH_GL_CALL(glBlitFramebuffer(0, 0, props.width, props.height, 0, 0, fbo->props.width, fbo->props.height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
		if (depth)
			MH_GL_CALL(glBlitFramebuffer(0, 0, props.width, props.height, 0, 0, fbo->props.width, fbo->props.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST));

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			MH_CORE_WARN("Attempted to resize framebuffer to an unsupported size: ({0},{1})", width, height);
			return;
		}

		props.width = width;
		props.height = height;

		Invalidate();
	}

	void OpenGLFrameBuffer::ReadColorPixels(void* pixels, int attachmentSlot) const
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(attachmentSlot < colorAttachments.size(), "Index outside range of framebuffer textures!");

		auto& spec = props.colorAttachments[attachmentSlot];

		GLenum format = TextureFormatToOpenGLInternalFormat(spec.format);
		GLenum type = TextureFormatToOpenGLType(spec.format);

		MH_GL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentSlot));

		MH_GL_CALL(glReadPixels(0, 0, props.width, props.height, format, type, pixels));
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		MH_PROFILE_FUNCTION();

		if (rendererID)
		{
			MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rendererID));

			for (int i = 0; i < colorAttachments.size(); i++)
			{
				colorAttachments[i]->Resize(props.width, props.height);
				MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachments[i]->GetRendererID(), 0));
			}

			if (!props.dontUseDepth && depthAttachment)
			{
				depthAttachment->Resize(props.width, props.height);

				uint32_t attachment = TextureFormatToOpenGLAttachment(props.depthAttachment.format);

				if (props.depthAttachment.immutable)
					MH_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, depthAttachment->GetRendererID()))
				else
					MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, depthAttachment->GetRendererID(), 0))
			}

			// Check framebuffer
			MH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

			MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

			return;
		}

		MH_GL_CALL(glGenFramebuffers(1, &rendererID));
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, rendererID));

		// Color buffers
		for (int i = 0; i < props.colorAttachments.size(); i++)
		{
			FrameBufferAttachmentProps& spec = props.colorAttachments[i];

			Asset<Texture> tex = Texture2D::Create({ props.width, props.height, spec.format, spec.filterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });

			colorAttachments.push_back(tex);

			MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex->GetRendererID(), 0));
		}

		// Depth buffer
		if (!props.dontUseDepth)
		{
			FrameBufferAttachmentProps& spec = props.depthAttachment;

			/*uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(spec.format);
			uint32_t type = TextureFormatToOpenGLType(spec.format);*/
			uint32_t attachment = TextureFormatToOpenGLAttachment(spec.format);

			if (spec.immutable)
			{
				depthAttachment = RenderBuffer::Create(props.width, props.height, spec.format);

				MH_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, depthAttachment->GetRendererID()));
			}
			else
			{
				depthAttachment = Texture2D::Create({ props.width, props.height, spec.format, spec.filterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false });

				MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, depthAttachment->GetRendererID(), 0));
			}
		}

		if (colorAttachments.size() > 1)
		{
			GLenum* buffers = new GLenum[colorAttachments.size()];
			for (GLenum i = 0; i < colorAttachments.size(); i++)
				buffers[i] = GL_COLOR_ATTACHMENT0 + i;

			MH_GL_CALL(glDrawBuffers((GLsizei)colorAttachments.size(), buffers));

			delete[] buffers;
		}
		else if (colorAttachments.empty())
		{
			MH_GL_CALL(glDrawBuffer(GL_NONE));
			MH_GL_CALL(glReadBuffer(GL_NONE));
		}

		// Check framebuffer
		MH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}