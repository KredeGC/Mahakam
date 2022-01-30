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

	static void setWrapMode(uint32_t rendererID, GLenum axis, TextureWrapMode wrapMode)
	{
		const float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		switch (wrapMode)
		{
		case TextureWrapMode::Repeat:
			glTextureParameteri(rendererID, axis, GL_REPEAT);
			break;
		case TextureWrapMode::Clamp:
			glTextureParameteri(rendererID, axis, GL_CLAMP_TO_EDGE);
			break;
		case TextureWrapMode::ClampBorder:
			glTextureParameterfv(rendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
			glTextureParameteri(rendererID, axis, GL_CLAMP_TO_BORDER);
			break;
		default:
			MH_CORE_BREAK("Unsupported TextureWrapMode!");
		}
	}

	static void setFilterMode(uint32_t rendererID, bool mipmaps, TextureFilter filterMode)
	{
		// Minification
		if (mipmaps)
		{
			if (filterMode == TextureFilter::Trilinear)
				glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else if (filterMode == TextureFilter::Bilinear)
				glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		else
		{
			if (filterMode == TextureFilter::Trilinear)
				glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			else if (filterMode == TextureFilter::Bilinear)
				glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			else
				glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		// Magnification
		if (filterMode == TextureFilter::Bilinear)
			glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		else
			glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLFormat(this->props.format);
		formatType = TextureFormatToOpenGLType(this->props.format);

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glTextureStorage2D(rendererID, 1, internalFormat, this->props.width, this->props.height);

		// Wrap X
		setWrapMode(rendererID, GL_TEXTURE_WRAP_S, this->props.wrapX);

		// Wrap Y
		setWrapMode(rendererID, GL_TEXTURE_WRAP_T, this->props.wrapY);

		// Filter mode & mipmaps
		setFilterMode(rendererID, props.mipmaps, props.filterMode);

		if (this->props.mipmaps)
			glGenerateTextureMipmap(rendererID);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, const TextureProps& props)
		: filepath(filepath), props(props)
	{
		MH_PROFILE_FUNCTION();

		int w, h, channels, hdr;
		void* data = loadImageFile(filepath.c_str(), &w, &h, &channels, &hdr);

		this->props.width = w;
		this->props.height = h;

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLFormat(this->props.format, channels);
		formatType = TextureFormatToOpenGLType(this->props.format);

		MH_CORE_ASSERT(internalFormat && dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
		glBindTexture(GL_TEXTURE_2D, rendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, this->props.width, this->props.height, 0, dataFormat, formatType, data);

		if (channels == 2) // Look some more into this, but keep it for now
		{
			const GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
			glTextureParameteriv(rendererID, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		}

		// Wrap X
		setWrapMode(rendererID, GL_TEXTURE_WRAP_S, this->props.wrapX);

		// Wrap Y
		setWrapMode(rendererID, GL_TEXTURE_WRAP_T, this->props.wrapY);

		// Filter mode & mipmaps
		setFilterMode(rendererID, props.mipmaps, props.filterMode);

		if (this->props.mipmaps)
			glGenerateTextureMipmap(rendererID);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		MH_PROFILE_FUNCTION();

		glDeleteTextures(1, &rendererID);
	}

	void OpenGLTexture2D::setData(void* data, bool mipmaps)
	{
		MH_PROFILE_FUNCTION();

		uint32_t bpp = TextureFormatToByteSize(props.format);
		glTextureSubImage2D(rendererID, 0, 0, 0, props.width, props.height, dataFormat, formatType, data);

		if (props.mipmaps)
			glGenerateTextureMipmap(rendererID);
	}

	void OpenGLTexture2D::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, rendererID);
	}

	void OpenGLTexture2D::readPixels(void* pixels, bool mipmaps)
	{
		// TODO: Make mips work
		MH_CORE_ASSERT(!mipmaps, "Mip maps not supported!");

		MH_PROFILE_FUNCTION();

		uint32_t bpp = TextureFormatToByteSize(props.format);
		uint32_t size = props.width * props.height * bpp;

		glGetTextureImage(rendererID, 0, dataFormat, formatType, size, pixels);
	}



	static Ref<Mesh> cubeMesh;

	OpenGLTextureCube::OpenGLTextureCube(const CubeTextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLFormat(this->props.format);
		formatType = TextureFormatToOpenGLType(this->props.format);

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);

		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				this->props.resolution, this->props.resolution, 0, dataFormat, formatType, nullptr);
		}

		// Wrap mode doesn't make sense on a cubemap
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Filter mode & mipmaps
		setFilterMode(rendererID, props.mipmaps, props.filterMode);

		if (this->props.mipmaps)
			glGenerateTextureMipmap(rendererID);
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::vector<std::string>& faces, const CubeTextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);

		internalFormat = 0, dataFormat = 0;
		int w = 0, h = 0, channels, hdr;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			void* data = loadImageFile(filepath.c_str(), &w, &h, &channels, &hdr);

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

		// Wrap mode doesn't make sense on a cubemap
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Filter mode & mipmaps
		setFilterMode(rendererID, props.mipmaps, props.filterMode);

		if (this->props.mipmaps)
			glGenerateTextureMipmap(rendererID);
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::string& filepath, const CubeTextureProps& props)
		: filepath(filepath), props(props)
	{
		MH_PROFILE_FUNCTION();

		if (!cubeMesh)
			cubeMesh = Mesh::createCube(2, true);

		int w, h, channels, hdr;
		void* data = loadImageFile(filepath.c_str(), &w, &h, &channels, &hdr);

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLFormat(this->props.format, channels);
		formatType = TextureFormatToOpenGLType(this->props.format);

		uint32_t hdrID;
		glGenTextures(1, &hdrID);
		glBindTexture(GL_TEXTURE_2D, hdrID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, dataFormat, hdr ? GL_FLOAT : formatType, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);
		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				this->props.resolution, this->props.resolution, 0, dataFormat, formatType, nullptr);
		}

		// Wrap
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Filter mode & mipmaps
		setFilterMode(rendererID, props.mipmaps, props.filterMode);

		if (this->props.mipmaps)
			glGenerateTextureMipmap(rendererID);


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
		OpenGLShader equiToCubeShader("assets/shaders/internal/Cubemap.glsl");
		equiToCubeShader.bind();
		equiToCubeShader.setUniformInt("equirectangularMap", 0);
		equiToCubeShader.setUniformMat4("projection", captureProjection);

		glBindTextureUnit(0, hdrID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrID);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

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


		// Generate mipmaps if not convoluting
		if (this->props.mipmaps)
			glGenerateTextureMipmap(rendererID);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);


		// Delete buffer
		glDeleteTextures(1, &hdrID);
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}

	OpenGLTextureCube::OpenGLTextureCube(Ref<TextureCube> cubemap, const CubeTextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		if (!cubeMesh)
			cubeMesh = Mesh::createCube(2, true);

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLFormat(this->props.format);
		formatType = TextureFormatToOpenGLType(this->props.format);

		// Create framebuffer
		uint32_t captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->props.resolution, this->props.resolution);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);


		// Create cubemap
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);
		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				this->props.resolution, this->props.resolution, 0, dataFormat, formatType, nullptr);
		}

		// Wrap
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(rendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		// Filter mode & mipmaps
		setFilterMode(rendererID, props.mipmaps, props.filterMode);

		if (this->props.mipmaps)
			glGenerateTextureMipmap(rendererID);


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
		equiToCubeShader.setUniformInt("environmentMap", 0);
		equiToCubeShader.setUniformMat4("projection", captureProjection);

		glBindTextureUnit(0, cubemap->getRendererID());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getRendererID());

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		if (this->props.mipmaps && this->props.prefilter == TextureCubePrefilter::Prefilter)
		{
			glGenerateTextureMipmap(rendererID);

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
			glGenerateTextureMipmap(rendererID);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);


		// Delete buffer
		glDeleteFramebuffers(1, &captureFBO);
		glDeleteRenderbuffers(1, &captureRBO);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		MH_PROFILE_FUNCTION();

		glDeleteTextures(1, &rendererID);
	}

	void OpenGLTextureCube::setData(void* data, bool mipmaps)
	{
		MH_PROFILE_FUNCTION();

		uint32_t bpp = TextureFormatToByteSize(props.format);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);

		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(props.resolution)));
		uint32_t maxMipLevels = mipmaps ? mipLevels : 1;

		uint32_t offset = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipResolution = (uint32_t)(this->props.resolution * std::pow(0.5, mip));

			for (uint32_t i = 0; i < 6; ++i)
			{
				glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, 0, 0,
					mipResolution, mipResolution, dataFormat, formatType, (char*)data + offset);

				offset += mipResolution * mipResolution * bpp;
			}
		}

		if (props.mipmaps && !mipmaps)
			glGenerateTextureMipmap(rendererID);
	}

	void OpenGLTextureCube::bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, rendererID);
	}

	void OpenGLTextureCube::readPixels(void* pixels, bool mipmaps)
	{
		MH_PROFILE_FUNCTION();

		uint32_t bpp = TextureFormatToByteSize(props.format);

		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(props.resolution)));
		uint32_t maxMipLevels = mipmaps ? mipLevels : 1;

		uint32_t offset = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipResolution = (uint32_t)(this->props.resolution * std::pow(0.5, mip));
			uint32_t size = 6 * mipResolution * mipResolution * bpp;

			glGetTextureImage(rendererID, mip, dataFormat, formatType, size, (char*)pixels + offset);

			offset += size;
		}
	}
}