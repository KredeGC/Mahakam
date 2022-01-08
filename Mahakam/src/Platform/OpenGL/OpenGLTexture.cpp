#include "mhpch.h"
#include "OpenGLTexture.h"

#include "OpenGLShader.h"

#include "Mahakam/Renderer/Mesh.h"

#include "OpenGLTextureFormats.h"

#include <stb_image.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mahakam
{
	static void* loadImageFile(const char* filepath, int* w, int* h, int* channels, int* hdr, int desiredChannels = 0)
	{
		MH_PROFILE_FUNCTION();

		stbi_set_flip_vertically_on_load(1);

		void* data = nullptr;
		*hdr = stbi_is_hdr(filepath);
		if (*hdr)
			data = stbi_loadf(filepath, w, h, channels, desiredChannels);
		else
			data = stbi_load(filepath, w, h, channels, desiredChannels);

		MH_CORE_ASSERT(data, "Failed to load image!");

		return data;
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLFormat(this->props.format);

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glTextureStorage2D(rendererID, 1, internalFormat, this->props.width, this->props.height);

		if (this->props.mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		// Wrap X
		if (this->props.wrapX == TextureWrapMode::Repeat)
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		// Wrap Y
		if (this->props.wrapX == TextureWrapMode::Repeat)
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Minification
		if (this->props.filterMode == TextureFilter::Bilinear)
			glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

		// Magnification
		if (this->props.filterMode == TextureFilter::Bilinear)
			glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, const TextureProps& props)
		: filepath(filepath), props(props)
	{
		MH_PROFILE_FUNCTION();

		int w, h, channels, hdr;
		void* data = loadImageFile(filepath.c_str(), &w, &h, &channels, &hdr);

		MH_CORE_ASSERT(data, "Failed to load image!");

		this->props.width = w;
		this->props.height = h;

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLFormat(this->props.format);
		GLenum formatType = TextureFormatToOpenGLType(this->props.format);

		MH_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		// TODO: Fix TextureFormatToOpenGLFormat() to take in the number of channels
		if (channels == 1)
			dataFormat = GL_R8;

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glBindTexture(GL_TEXTURE_2D, rendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, this->props.width, this->props.height, 0, dataFormat, formatType, data);

		if (this->props.mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		// Wrap X
		if (this->props.wrapX == TextureWrapMode::Repeat)
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		// Wrap Y
		if (this->props.wrapX == TextureWrapMode::Repeat)
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Minification
		if (this->props.filterMode == TextureFilter::Bilinear)
			glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

		// Magnification
		if (this->props.filterMode == TextureFilter::Bilinear)
			glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		MH_PROFILE_FUNCTION();

		glDeleteTextures(1, &rendererID);
	}

	void OpenGLTexture2D::setData(void* data, uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		uint32_t bpp = dataFormat == GL_RGBA ? 4 : 3;
		MH_CORE_ASSERT(size == props.width * props.height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(rendererID, 0, 0, 0, props.width, props.height, dataFormat, GL_UNSIGNED_BYTE, data);

		if (props.mipmaps)
			glGenerateTextureMipmap(rendererID);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		MH_PROFILE_FUNCTION();

		glBindTextureUnit(slot, rendererID);
	}



	static Ref<Mesh> cubeMesh;

	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& faces, const CubeTextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		glGenTextures(1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);

		internalFormat = 0, dataFormat = 0;
		int w = 0, h = 0, channels, hdr;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			void* data = loadImageFile(filepath.c_str(), &w, &h, &channels, &hdr);

			MH_CORE_ASSERT(data, "Failed to load image!");

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

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, w, h, 0, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		if (this->props.mipmaps)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::string& filepath, const CubeTextureProps& props)
		: filepath(filepath), props(props)
	{
		if (!cubeMesh)
			cubeMesh = Mesh::createCube(2);

		MH_PROFILE_FUNCTION();

		int w, h, channels, hdr;
		void* data = loadImageFile(filepath.c_str(), &w, &h, &channels, &hdr);

		this->props.resolution = this->props.resolution;

		MH_CORE_ASSERT(data, "Failed to load image!");

		internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA16F;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB16F;
			dataFormat = GL_RGB;
		}

		uint32_t hdrID;
		glGenTextures(1, &hdrID);
		glBindTexture(GL_TEXTURE_2D, hdrID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, dataFormat, GL_FLOAT, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(data);


		// Create framebuffer
		uint32_t captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->props.resolution, this->props.resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


		// Create cubemap
		glGenTextures(1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);
		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				this->props.resolution, this->props.resolution, 0, dataFormat, GL_FLOAT, nullptr);
		}

		if (this->props.mipmaps)
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		// Create matrices
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};


		// Convert HDR equirectangular environment map to cubemap equivalent
		std::string shaderPath = "assets/shaders/internal/";
		if (this->props.prefilter == TextureCubePrefilter::Convolute)
			shaderPath += "CubemapBlur.glsl";
		else if (this->props.prefilter == TextureCubePrefilter::Prefilter)
			shaderPath += "CubemapSpec.glsl";
		else
			shaderPath += "Cubemap.glsl";
		OpenGLShader equiToCubeShader(shaderPath);
		equiToCubeShader.bind();
		equiToCubeShader.setUniformInt("equirectangularMap", 0);
		equiToCubeShader.setUniformMat4("projection", captureProjection);

		glBindTextureUnit(0, hdrID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrID);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		if (this->props.mipmaps && this->props.prefilter == TextureCubePrefilter::Prefilter)
		{
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				// Resize framebuffer according to mip-level size.
				uint32_t mipWidth = (uint32_t)(this->props.resolution * std::pow(0.5, mip));
				uint32_t mipHeight = (uint32_t)(this->props.resolution * std::pow(0.5, mip));
				glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
				glViewport(0, 0, mipWidth, mipHeight);

				float roughness = (float)mip / (float)(maxMipLevels - 1);
				equiToCubeShader.setUniformFloat("roughness", roughness);
				for (unsigned int i = 0; i < 6; ++i)
				{
					equiToCubeShader.setUniformMat4("view", captureViews[i]);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rendererID, mip);

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					cubeMesh->bind();
					glDrawElements(GL_TRIANGLES, cubeMesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
					cubeMesh->unbind();
				}
			}
		}
		else
		{
			glViewport(0, 0, this->props.resolution, this->props.resolution); // don't forget to configure the viewport to the capture dimensions.
			for (unsigned int i = 0; i < 6; ++i)
			{
				equiToCubeShader.setUniformMat4("view", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rendererID, 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				cubeMesh->bind();
				glDrawElements(GL_TRIANGLES, cubeMesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
				cubeMesh->unbind();
			}
		}


		// Generate mipmaps if not convoluting
		if (this->props.mipmaps && this->props.prefilter != TextureCubePrefilter::Prefilter)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);


		// Delete buffer
		glDeleteTextures(1, &hdrID);
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		MH_PROFILE_FUNCTION();

		glDeleteTextures(1, &rendererID);
	}

	void OpenGLTextureCube::setData(void* data, uint32_t size)
	{
		MH_PROFILE_FUNCTION();

		uint32_t bpp = dataFormat == GL_RGBA ? 4 : 3;
		MH_CORE_ASSERT(size == props.resolution * props.resolution * bpp, "Data must be entire texture!");
		glTextureSubImage2D(rendererID, 0, 0, 0, props.resolution, props.resolution, dataFormat, GL_UNSIGNED_BYTE, data);

		if (props.mipmaps)
			glGenerateTextureMipmap(rendererID);
	}

	void OpenGLTextureCube::bind(uint32_t slot) const
	{
		MH_PROFILE_FUNCTION();

		glBindTextureUnit(slot, rendererID);
	}
}