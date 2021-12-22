#include "mhpch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath) : filepath(filepath)
	{
		int w, h, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(filepath.c_str(), &w, &h, &channels, 0);

		MH_CORE_ASSERT(data, "Failed to load image!");

		width = w;
		height = h;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		MH_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glTextureStorage2D(rendererID, 1, internalFormat, width, height);

		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &rendererID);
	}
	
	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, rendererID);
	}
}