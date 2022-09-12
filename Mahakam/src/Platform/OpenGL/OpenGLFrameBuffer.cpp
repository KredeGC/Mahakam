#include "Mahakam/mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLFrameBuffer.h"

#include "OpenGLTextureFormats.h"

#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Renderer/RenderBuffer.h"
#include "Mahakam/Renderer/Texture.h"

#include <glad/glad.h>

namespace Mahakam
{
	static const uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferProps& props)
		: m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		if (!props.SwapChainTarget)
			Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteFramebuffers(1, &m_RendererID));
	}

	void OpenGLFrameBuffer::Bind()
	{
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
		MH_GL_CALL(glViewport(0, 0, m_Props.Width, m_Props.Height));
	}

	void OpenGLFrameBuffer::Unbind()
	{
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void OpenGLFrameBuffer::Blit(Ref<FrameBuffer> dest, bool color, bool depth)
	{
		Ref<OpenGLFrameBuffer> fbo = StaticCastRef<OpenGLFrameBuffer>(dest);

		MH_GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID));
		MH_GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->m_RendererID));

		if (color)
			MH_GL_CALL(glBlitFramebuffer(0, 0, m_Props.Width, m_Props.Height, 0, 0, fbo->m_Props.Width, fbo->m_Props.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
		if (depth)
			MH_GL_CALL(glBlitFramebuffer(0, 0, m_Props.Width, m_Props.Height, 0, 0, fbo->m_Props.Width, fbo->m_Props.Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST));

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
		{
			MH_CORE_WARN("Attempted to resize framebuffer to an unsupported size: ({0},{1})", width, height);
			return;
		}

		m_Props.Width = width;
		m_Props.Height = height;

		if (!m_Props.SwapChainTarget)
			Invalidate();
	}

	Asset<Texture> OpenGLFrameBuffer::GetColorTexture(int index) const
	{
		return m_ColorAttachments[index];
	}

	Asset<Texture> OpenGLFrameBuffer::GetDepthTexture() const
	{
		return m_DepthAttachment;
	}

	void OpenGLFrameBuffer::ReadColorPixels(void* pixels, int attachmentSlot) const
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(attachmentSlot < m_ColorAttachments.size(), "Index outside range of framebuffer textures!");

		auto& spec = m_Props.ColorAttachments[attachmentSlot];

		GLenum format = TextureFormatToOpenGLInternalFormat(spec.Format);
		GLenum type = TextureFormatToOpenGLType(spec.Format);

		MH_GL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentSlot));

		MH_GL_CALL(glReadPixels(0, 0, m_Props.Width, m_Props.Height, format, type, pixels));
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		MH_PROFILE_FUNCTION();

		if (m_RendererID)
		{
			MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

			for (int i = 0; i < m_ColorAttachments.size(); i++)
			{
				m_ColorAttachments[i]->Resize(m_Props.Width, m_Props.Height);
				MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i]->GetRendererID(), 0));
			}

			if (!m_Props.DontUseDepth && m_DepthAttachment)
			{
				m_DepthAttachment->Resize(m_Props.Width, m_Props.Height);

				uint32_t attachment = TextureFormatToOpenGLAttachment(m_Props.DepthAttachment.Format);

				if (m_Props.DepthAttachment.Immutable)
					MH_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_DepthAttachment->GetRendererID()))
				else
					MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_DepthAttachment->GetRendererID(), 0))
			}

			// Check framebuffer
			MH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

			MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

			return;
		}

		MH_GL_CALL(glGenFramebuffers(1, &m_RendererID));
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));

		// Color buffers
		for (int i = 0; i < m_Props.ColorAttachments.size(); i++)
		{
			FrameBufferAttachmentProps& spec = m_Props.ColorAttachments[i];

			Asset<Texture> tex = Asset<Texture2D>(Texture2D::Create({ m_Props.Width, m_Props.Height, spec.Format, spec.FilterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false }));

			m_ColorAttachments.push_back(tex);

			MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex->GetRendererID(), 0));
		}

		// Depth buffer
		if (!m_Props.DontUseDepth)
		{
			FrameBufferAttachmentProps& spec = m_Props.DepthAttachment;

			/*uint32_t internalFormat = TextureFormatToOpenGLInternalFormat(spec.format);
			uint32_t type = TextureFormatToOpenGLType(spec.format);*/
			uint32_t attachment = TextureFormatToOpenGLAttachment(spec.Format);

			if (spec.Immutable)
			{
				m_DepthAttachment = RenderBuffer::Create(m_Props.Width, m_Props.Height, spec.Format);

				MH_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_DepthAttachment->GetRendererID()));
			}
			else
			{
				m_DepthAttachment = Asset<Texture2D>(Texture2D::Create({ m_Props.Width, m_Props.Height, spec.Format, spec.FilterMode, TextureWrapMode::Clamp, TextureWrapMode::Clamp, false }));

				MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_DepthAttachment->GetRendererID(), 0));
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			GLenum* buffers = new GLenum[m_ColorAttachments.size()];
			for (GLenum i = 0; i < m_ColorAttachments.size(); i++)
				buffers[i] = GL_COLOR_ATTACHMENT0 + i;

			MH_GL_CALL(glDrawBuffers((GLsizei)m_ColorAttachments.size(), buffers));

			delete[] buffers;
		}
		else if (m_ColorAttachments.empty())
		{
			MH_GL_CALL(glDrawBuffer(GL_NONE));
			MH_GL_CALL(glReadBuffer(GL_NONE));
		}

		// Check framebuffer
		MH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}