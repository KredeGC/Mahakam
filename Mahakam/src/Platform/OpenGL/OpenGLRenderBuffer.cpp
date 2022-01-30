#include "mhpch.h"
#include "OpenGLRenderBuffer.h"

#include "OpenGLTextureFormats.h"

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLRenderBuffer::OpenGLRenderBuffer(uint32_t width, uint32_t height, TextureFormat format)
		: width(width), height(height)
	{
		internalFormat = TextureFormatToOpenGLInternalFormat(format, false);

		glCreateRenderbuffers(1, &rendererID);
		glNamedRenderbufferStorage(rendererID, internalFormat, width, height);
	}

	OpenGLRenderBuffer::~OpenGLRenderBuffer()
	{
		glDeleteRenderbuffers(1, &rendererID);
	}
}