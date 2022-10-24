#include "Mahakam/mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLRenderBuffer.h"

#include "OpenGLTextureFormats.h"

#include <glad/gl.h>

namespace Mahakam
{
	OpenGLRenderBuffer::OpenGLRenderBuffer(uint32_t width, uint32_t height, TextureFormat format)
		: m_Width(width), m_Height(height), m_Size(width * height * TextureFormatToByteSize(format))
	{
		m_InternalFormat = TextureFormatToOpenGLInternalFormat(format);

		MH_GL_CALL(glGenRenderbuffers(1, &m_RendererID));
		MH_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID));
		MH_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, m_InternalFormat, width, height));
	}

	OpenGLRenderBuffer::~OpenGLRenderBuffer()
	{
		MH_GL_CALL(glDeleteRenderbuffers(1, &m_RendererID));
	}

	void OpenGLRenderBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		MH_GL_CALL(glDeleteRenderbuffers(1, &m_RendererID));

		MH_GL_CALL(glGenRenderbuffers(1, &m_RendererID));
		MH_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID));
		MH_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, m_InternalFormat, m_Width, m_Height));
	}
}