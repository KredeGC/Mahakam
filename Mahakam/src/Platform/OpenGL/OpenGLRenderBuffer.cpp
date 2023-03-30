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

	OpenGLRenderBuffer::OpenGLRenderBuffer(const OpenGLRenderBuffer& other) :
		m_Width(other.m_Width),
		m_Height(other.m_Height),
		m_InternalFormat(other.m_InternalFormat),
		m_Size(other.m_Size)
	{
		MH_BREAK("Copying textures is currently not supported!");
	}

	OpenGLRenderBuffer::OpenGLRenderBuffer(OpenGLRenderBuffer&& other) noexcept :
		m_RendererID(other.m_RendererID),
		m_Width(other.m_Width),
		m_Height(other.m_Height),
		m_InternalFormat(other.m_InternalFormat),
		m_Size(other.m_Size)
	{
		other.m_RendererID = 0;
		other.m_Size = 0;
	}

	OpenGLRenderBuffer::~OpenGLRenderBuffer()
	{
		if (m_RendererID)
			MH_GL_CALL(glDeleteRenderbuffers(1, &m_RendererID));
	}

	OpenGLRenderBuffer& OpenGLRenderBuffer::operator=(const OpenGLRenderBuffer& rhs)
	{
		MH_BREAK("Copying textures is currently not supported!");

		return *this;
	}

	OpenGLRenderBuffer& OpenGLRenderBuffer::operator=(OpenGLRenderBuffer&& rhs) noexcept
	{
		m_RendererID = rhs.m_RendererID;
		m_Width = rhs.m_Width;
		m_Height = rhs.m_Height;
		m_InternalFormat = rhs.m_InternalFormat;
		m_Size = rhs.m_Size;

		rhs.m_RendererID = 0;
		rhs.m_Size = 0;

		return *this;
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