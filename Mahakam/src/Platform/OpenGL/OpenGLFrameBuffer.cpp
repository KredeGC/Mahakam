#include "mhpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferProps& prop)
		: prop(prop)
	{
		colorAttachments.push_back(0);

		invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &rendererID);

		for (int i = 0; i < colorAttachments.size(); i++)
			glDeleteTextures(1, &colorAttachments[i]);

		glDeleteTextures(1, &depthAttachment);
	}

	void OpenGLFrameBuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);
		glViewport(0, 0, prop.width, prop.height);
	}

	void OpenGLFrameBuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::resize(uint32_t width, uint32_t height)
	{
		prop.width = width;
		prop.height = height;

		invalidate();
	}

	void OpenGLFrameBuffer::invalidate()
	{
		if (rendererID)
		{
			glDeleteFramebuffers(1, &rendererID);

			for (int i = 0; i < colorAttachments.size(); i++)
				glDeleteTextures(1, &colorAttachments[i]);

			glDeleteTextures(1, &depthAttachment);
		}

		glCreateFramebuffers(1, &rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

		// Color buffer
		glCreateTextures(GL_TEXTURE_2D, 1, &colorAttachments[0]);
		glBindTexture(GL_TEXTURE_2D, colorAttachments[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, prop.width, prop.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachments[0], 0);

		// Depth buffer
		glCreateTextures(GL_TEXTURE_2D, 1, &depthAttachment);
		glTextureStorage2D(depthAttachment, 1, GL_DEPTH24_STENCIL8, prop.width, prop.height);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, prop.width, prop.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

		// Bind framebuffer
		MH_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}