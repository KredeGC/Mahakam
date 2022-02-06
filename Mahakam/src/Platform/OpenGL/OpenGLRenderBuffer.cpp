#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLRenderBuffer.h"

#include "OpenGLTextureFormats.h"

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLRenderBuffer::OpenGLRenderBuffer(uint32_t width, uint32_t height, TextureFormat format)
		: width(width), height(height), size(width * height * TextureFormatToByteSize(format))
	{
		internalFormat = TextureFormatToOpenGLInternalFormat(format);

		MH_GL_CALL(glCreateRenderbuffers(1, &rendererID));
		MH_GL_CALL(glNamedRenderbufferStorage(rendererID, internalFormat, width, height));
	}

	OpenGLRenderBuffer::~OpenGLRenderBuffer()
	{
		MH_GL_CALL(glDeleteRenderbuffers(1, &rendererID));
	}

	void OpenGLRenderBuffer::Resize(uint32_t width, uint32_t height)
	{
		this->width = width;
		this->height = height;

		MH_GL_CALL(glDeleteRenderbuffers(1, &rendererID));

		MH_GL_CALL(glCreateRenderbuffers(1, &rendererID));
		MH_GL_CALL(glNamedRenderbufferStorage(rendererID, internalFormat, width, height));
	}
}