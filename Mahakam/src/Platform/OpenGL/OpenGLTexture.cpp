#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureFormats.h"

#include "OpenGLShader.h"

#include "Mahakam/Renderer/GL.h"

#include <stb_image.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mahakam
{
	static void* LoadImageFile(const char* filepath, int* w, int* h, int* channels, int* hdr, int desiredChannels = 0)
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

	static void SetWrapMode(uint32_t targetID, GLenum axis, TextureWrapMode wrapMode)
	{
		const float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		switch (wrapMode)
		{
		case TextureWrapMode::Repeat:
			MH_GL_CALL(glTexParameteri(targetID, axis, GL_REPEAT));
			break;
		case TextureWrapMode::Clamp:
			MH_GL_CALL(glTexParameteri(targetID, axis, GL_CLAMP_TO_EDGE));
			break;
		case TextureWrapMode::ClampBorder:
			MH_GL_CALL(glTexParameterfv(targetID, GL_TEXTURE_BORDER_COLOR, borderColor));
			MH_GL_CALL(glTexParameteri(targetID, axis, GL_CLAMP_TO_BORDER));
			break;
		default:
			MH_CORE_BREAK("Unsupported TextureWrapMode!");
		}
	}

	static void SetFilterMode(uint32_t targetID, bool mipmaps, TextureFilter filterMode)
	{
		// Minification
		if (mipmaps)
		{
			if (filterMode == TextureFilter::Trilinear)
				MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR))
			else if (filterMode == TextureFilter::Bilinear)
				MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST))
			else
				MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST))
		}
		else
		{
			if (filterMode == TextureFilter::Trilinear)
				MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
			else if (filterMode == TextureFilter::Bilinear)
				MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
			else
				MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MIN_FILTER, GL_NEAREST))
		}

		// Magnification
		if (filterMode == TextureFilter::Bilinear)
			MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
		else
			MH_GL_CALL(glTexParameteri(targetID, GL_TEXTURE_MAG_FILTER, GL_NEAREST))
	}

	static uint32_t CalculateTextureByteSize(uint32_t targetID, uint32_t bpp, bool compressed, bool mipmaps, uint32_t width, uint32_t height)
	{
		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(std::max(width, height))));
		uint32_t maxMipLevels = (!compressed && mipmaps) ? mipLevels : 1; // TODO: Support compressed mipmaps

		uint32_t size = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipSize;
			if (compressed)
			{
				int compressed_size;
				MH_GL_CALL(glGetTexLevelParameteriv(targetID, mip, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size));

				mipSize = compressed_size;
			}
			else
			{
				uint32_t mipWidth = (uint32_t)(width * std::pow(0.5, mip));
				uint32_t mipHeight = (uint32_t)(height * std::pow(0.5, mip));

				mipSize = mipWidth * mipHeight * bpp;
			}

			size += mipSize;
		}

		return size;
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureProps& props)
		: m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		internalFormat = TextureFormatToOpenGLInternalFormat(props.format);
		dataFormat = TextureFormatToOpenGLBaseFormat(props.format);
		formatType = TextureFormatToOpenGLType(props.format);
		compressed = IsTextureFormatCompressed(props.format);

		Init();
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& filepath, const TextureProps& props)
		: filepath(filepath), m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		int w, h, channels, hdr;
		void* data = LoadImageFile(this->filepath.string().c_str(), &w, &h, &channels, &hdr);

		m_Props.width = w;
		m_Props.height = h;

		internalFormat = TextureFormatToOpenGLInternalFormat(m_Props.format);
		dataFormat = ChannelCountToOpenGLBaseFormat(channels);
		formatType = TextureFormatToOpenGLType(m_Props.format);
		compressed = IsTextureFormatCompressed(m_Props.format);

		// Create and populate the texture
		MH_GL_CALL(glGenTextures(1, &rendererID));
		MH_GL_CALL(glBindTexture(GL_TEXTURE_2D, rendererID));
		MH_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Props.width, m_Props.height, 0, dataFormat, formatType, data));

		if (channels == 2) // TODO: Look some more into this, but keep it for now
		{
			const GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_GREEN };
			MH_GL_CALL(glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask));
		}

		// Wrap X
		SetWrapMode(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Props.wrapX);

		// Wrap Y
		SetWrapMode(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Props.wrapY);

		// Filter mode & mipmaps
		SetFilterMode(GL_TEXTURE_2D, m_Props.mipmaps, m_Props.filterMode);

		if (m_Props.mipmaps)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

		// Calculate the size
		dataFormat = TextureFormatToOpenGLBaseFormat(m_Props.format); // Should this just be this way by default? idk
		uint32_t bpp = TextureFormatToByteSize(m_Props.format);
		size = CalculateTextureByteSize(GL_TEXTURE_2D, bpp, compressed, false, m_Props.width, m_Props.height);
		totalSize = CalculateTextureByteSize(GL_TEXTURE_2D, bpp, compressed, m_Props.mipmaps, m_Props.width, m_Props.height);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteTextures(1, &rendererID));
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size, bool mipmaps)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(!mipmaps || !compressed, "Compressed mipmaps are currently not supported!");

		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(std::max(m_Props.width, m_Props.height))));
		uint32_t maxMipLevels = mipmaps ? mipLevels : 1;

		uint32_t offset = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipWidth = (uint32_t)(m_Props.width * std::pow(0.5, mip));
			uint32_t mipHeight = (uint32_t)(m_Props.height * std::pow(0.5, mip));

			if (compressed)
				MH_GL_CALL(glCompressedTexSubImage2D(GL_TEXTURE_2D, mip, 0, 0, mipWidth, mipHeight, dataFormat, size, (char*)data + offset))
			else
				MH_GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, mip, 0, 0, mipWidth, mipHeight, dataFormat, formatType, (char*)data + offset));

			offset += size;
			size /= 4;
		}

		if (m_Props.mipmaps && !mipmaps)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		MH_GL_CALL(glBindTextureUnit(slot, rendererID));
	}

	void OpenGLTexture2D::BindImage(uint32_t slot, bool read, bool write) const
	{
		uint32_t readWrite = (read && write) ? GL_READ_WRITE : (write ? GL_WRITE_ONLY : GL_READ_ONLY);

		glBindImageTexture(slot, rendererID, 0, GL_FALSE, 0, readWrite, internalFormat);
	}

	void OpenGLTexture2D::ReadPixels(void* pixels, bool mipmaps)
	{
		MH_PROFILE_FUNCTION();

		uint32_t bpp = TextureFormatToByteSize(m_Props.format);

		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(std::max(m_Props.width, m_Props.height))));
		uint32_t maxMipLevels = mipmaps ? mipLevels : 1;

		uint32_t offset = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipSize;
			if (compressed)
			{
				int compressed_size;
				MH_GL_CALL(glGetTexLevelParameteriv(GL_TEXTURE_2D, mip, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size));

				mipSize = compressed_size;

				MH_GL_CALL(glGetCompressedTexImage(GL_TEXTURE_2D, mip, (char*)pixels + offset));
			}
			else
			{
				uint32_t mipWidth = (uint32_t)(m_Props.width * std::pow(0.5, mip));
				uint32_t mipHeight = (uint32_t)(m_Props.height * std::pow(0.5, mip));

				mipSize = mipWidth * mipHeight * bpp;
				MH_GL_CALL(glGetTexImage(GL_TEXTURE_2D, mip, dataFormat, formatType, (char*)pixels + offset));
			}

			offset += mipSize;
		}
	}

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteTextures(1, &rendererID));

		m_Props.width = width;
		m_Props.height = height;

		Init();
	}

	void OpenGLTexture2D::Init()
	{
		// Create an empty texture
		MH_GL_CALL(glGenTextures(1, &rendererID));
		MH_GL_CALL(glBindTexture(GL_TEXTURE_2D, rendererID));
		//glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Props.width, m_Props.height, 0, dataFormat, formatType, nullptr);
		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(std::max(m_Props.width, m_Props.height))));
		MH_GL_CALL(glTexStorage2D(GL_TEXTURE_2D, m_Props.mipmaps ? mipLevels : 1, internalFormat, m_Props.width, m_Props.height));

		// Wrap X
		SetWrapMode(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Props.wrapX);

		// Wrap Y
		SetWrapMode(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Props.wrapY);

		// Filter mode & mipmaps
		SetFilterMode(GL_TEXTURE_2D, m_Props.mipmaps, m_Props.filterMode);

		if (m_Props.mipmaps)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

		// Calculate the size
		uint32_t bpp = TextureFormatToByteSize(m_Props.format);
		size = CalculateTextureByteSize(GL_TEXTURE_2D, bpp, false, false, m_Props.width, m_Props.height);
		totalSize = CalculateTextureByteSize(GL_TEXTURE_2D, bpp, false, m_Props.mipmaps, m_Props.width, m_Props.height);
	}



	OpenGLTextureCube::OpenGLTextureCube(const CubeTextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLBaseFormat(this->props.format);
		formatType = TextureFormatToOpenGLType(this->props.format);
		compressed = IsTextureFormatCompressed(this->props.format);

		MH_GL_CALL(glGenTextures(1, &rendererID));
		MH_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID));

		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			MH_GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				this->props.resolution, this->props.resolution, 0, dataFormat, formatType, nullptr));
		}

		// Wrap mode doesn't make sense on a cubemap
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

		// Filter mode & mipmaps
		SetFilterMode(GL_TEXTURE_CUBE_MAP, this->props.mipmaps, this->props.filterMode);

		if (this->props.mipmaps)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

		// Calculate the size
		uint32_t bpp = TextureFormatToByteSize(this->props.format);
		size = CalculateTextureByteSize(GL_TEXTURE_CUBE_MAP, bpp, false, false, this->props.resolution, this->props.resolution);
		totalSize = 6 * CalculateTextureByteSize(GL_TEXTURE_CUBE_MAP, bpp, false, this->props.mipmaps, this->props.resolution, this->props.resolution);
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::filesystem::path& filepath, const CubeTextureProps& props)
		: filepath(filepath), props(props)
	{
		MH_PROFILE_FUNCTION();

		int w, h, channels, hdr;
		void* data = LoadImageFile(this->filepath.string().c_str(), &w, &h, &channels, &hdr);

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = ChannelCountToOpenGLBaseFormat(channels);
		formatType = TextureFormatToOpenGLType(this->props.format);
		compressed = IsTextureFormatCompressed(this->props.format);

		uint32_t hdrID;
		MH_GL_CALL(glGenTextures(1, &hdrID));
		MH_GL_CALL(glBindTexture(GL_TEXTURE_2D, hdrID));
		MH_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, dataFormat, hdr ? GL_FLOAT : formatType, data));
		
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		stbi_image_free(data);


		// Create framebuffer
		uint32_t captureFBO, captureRBO;
		MH_GL_CALL(glGenFramebuffers(1, &captureFBO));
		MH_GL_CALL(glGenRenderbuffers(1, &captureRBO));

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
		MH_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
		MH_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->props.resolution, this->props.resolution));
		MH_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO));


		// Create cubemap
		MH_GL_CALL(glGenTextures(1, &rendererID));
		MH_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID));
		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			MH_GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				this->props.resolution, this->props.resolution, 0, dataFormat, formatType, nullptr));
		}

		// Wrap
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

		// Filter mode & mipmaps
		SetFilterMode(GL_TEXTURE_CUBE_MAP, this->props.mipmaps, this->props.filterMode);

		/*if (this->props.mipmaps)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));*/


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
		OpenGLShader equiToCubeShader("assets/shaders/internal/Cubemap.shader");
		equiToCubeShader.Bind("LUT");
		equiToCubeShader.SetUniformInt("equirectangularMap", 0);
		equiToCubeShader.SetUniformMat4("projection", captureProjection);

		MH_GL_CALL(glBindTextureUnit(0, hdrID));
		//MH_GL_CALL(glActiveTexture(GL_TEXTURE0));
		//MH_GL_CALL(glBindTexture(GL_TEXTURE_2D, hdrID));

		GLint viewport[4];
		MH_GL_CALL(glGetIntegerv(GL_VIEWPORT, viewport));
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));

		MH_GL_CALL(glViewport(0, 0, this->props.resolution, this->props.resolution));
		for (unsigned int i = 0; i < 6; ++i)
		{
			equiToCubeShader.SetUniformMat4("view", captureViews[i]);
			MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rendererID, 0));

			MH_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			GL::GetInvertedCube()->Bind();
			MH_GL_CALL(glDrawElements(GL_TRIANGLES, GL::GetInvertedCube()->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
			GL::GetInvertedCube()->Unbind();
		}


		// Generate mipmaps
		if (this->props.mipmaps)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

		// Calculate the size
		uint32_t bpp = TextureFormatToByteSize(this->props.format);
		size = CalculateTextureByteSize(GL_TEXTURE_CUBE_MAP, bpp, compressed, false, this->props.resolution, this->props.resolution);
		totalSize = 6 * CalculateTextureByteSize(GL_TEXTURE_CUBE_MAP, bpp, compressed, this->props.mipmaps, this->props.resolution, this->props.resolution);

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		MH_GL_CALL(glViewport(viewport[0], viewport[1], viewport[2], viewport[3]));


		// Delete buffer
		MH_GL_CALL(glDeleteTextures(1, &hdrID));
		MH_GL_CALL(glDeleteFramebuffers(1, &captureFBO));
		MH_GL_CALL(glDeleteRenderbuffers(1, &captureRBO));
	}

	OpenGLTextureCube::OpenGLTextureCube(Asset<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props)
		: props(props)
	{
		MH_PROFILE_FUNCTION();

		internalFormat = TextureFormatToOpenGLInternalFormat(this->props.format);
		dataFormat = TextureFormatToOpenGLBaseFormat(this->props.format); //std::static_pointer_cast<OpenGLTextureCube>(cubemap)->dataFormat;
		formatType = TextureFormatToOpenGLType(this->props.format); //std::static_pointer_cast<OpenGLTextureCube>(cubemap)->formatType;
		compressed = IsTextureFormatCompressed(this->props.format);

		// Create framebuffer
		uint32_t captureFBO, captureRBO;
		MH_GL_CALL(glGenFramebuffers(1, &captureFBO));
		MH_GL_CALL(glGenRenderbuffers(1, &captureRBO));

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
		MH_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
		MH_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, this->props.resolution, this->props.resolution));
		MH_GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO));


		// Create cubemap
		MH_GL_CALL(glGenTextures(1, &rendererID));
		MH_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID));
		for (uint32_t i = 0; i < 6; ++i)
		{
			// note that we store each face with 16 bit floating point values
			MH_GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				this->props.resolution, this->props.resolution, 0, dataFormat, formatType, nullptr));
		}

		// Wrap
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		MH_GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

		// Filter mode & mipmaps
		SetFilterMode(GL_TEXTURE_CUBE_MAP, this->props.mipmaps, this->props.filterMode);

		// Generate mips before prefiltering
		if (this->props.mipmaps && prefilter == TextureCubePrefilter::Prefilter)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));


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
		if (prefilter == TextureCubePrefilter::Convolute)
			shaderPath += "CubemapBlur.shader";
		else if (prefilter == TextureCubePrefilter::Prefilter)
			shaderPath += "CubemapSpec.shader";
		OpenGLShader equiToCubeShader(shaderPath);
		equiToCubeShader.Bind("LUT");
		equiToCubeShader.SetUniformInt("environmentMap", 0);
		equiToCubeShader.SetUniformMat4("projection", captureProjection);

		MH_GL_CALL(glBindTextureUnit(0, cubemap->GetRendererID()));
		//MH_GL_CALL(glActiveTexture(GL_TEXTURE0));
		//MH_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->GetRendererID()));

		GLint viewport[4];
		MH_GL_CALL(glGetIntegerv(GL_VIEWPORT, viewport));
		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));

		if (this->props.mipmaps && prefilter == TextureCubePrefilter::Prefilter)
		{
			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				// Resize framebuffer according to mip-level size.
				uint32_t mipWidth = (uint32_t)(this->props.resolution * std::pow(0.5, mip));
				uint32_t mipHeight = (uint32_t)(this->props.resolution * std::pow(0.5, mip));
				MH_GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
				MH_GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight));
				MH_GL_CALL(glViewport(0, 0, mipWidth, mipHeight));

				float roughness = (float)mip / (float)(maxMipLevels - 1);
				equiToCubeShader.SetUniformFloat("roughness", roughness);
				for (unsigned int i = 0; i < 6; ++i)
				{
					equiToCubeShader.SetUniformMat4("view", captureViews[i]);
					MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rendererID, mip));

					MH_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

					GL::GetInvertedCube()->Bind();
					MH_GL_CALL(glDrawElements(GL_TRIANGLES, GL::GetInvertedCube()->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
					GL::GetInvertedCube()->Unbind();
				}
			}
		}
		else
		{
			MH_GL_CALL(glViewport(0, 0, this->props.resolution, this->props.resolution)); // don't forget to configure the viewport to the capture dimensions.
			for (unsigned int i = 0; i < 6; ++i)
			{
				equiToCubeShader.SetUniformMat4("view", captureViews[i]);
				MH_GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, rendererID, 0));

				MH_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

				GL::GetInvertedCube()->Bind();
				MH_GL_CALL(glDrawElements(GL_TRIANGLES, GL::GetInvertedCube()->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
				GL::GetInvertedCube()->Unbind();
			}
		}

		MH_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		MH_GL_CALL(glViewport(viewport[0], viewport[1], viewport[2], viewport[3]));


		// Delete buffer
		MH_GL_CALL(glDeleteFramebuffers(1, &captureFBO));
		MH_GL_CALL(glDeleteRenderbuffers(1, &captureRBO));


		// Generate mipmaps if not prefiltering
		if (this->props.mipmaps && prefilter != TextureCubePrefilter::Prefilter)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

		// Calculate the size
		uint32_t bpp = TextureFormatToByteSize(this->props.format);
		size = CalculateTextureByteSize(GL_TEXTURE_CUBE_MAP, bpp, compressed, false, this->props.resolution, this->props.resolution);
		totalSize = 6 * CalculateTextureByteSize(GL_TEXTURE_CUBE_MAP, bpp, compressed, this->props.mipmaps, this->props.resolution, this->props.resolution);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glDeleteTextures(1, &rendererID));
	}

	void OpenGLTextureCube::Resize(uint32_t width, uint32_t height)
	{
		MH_CORE_BREAK("UNSUPPORTED");
	}

	void OpenGLTextureCube::SetData(void* data, uint32_t size, bool mipmaps)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(!mipmaps || !compressed, "Compressed mipmaps are currently not supported!");

		MH_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID));

		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(props.resolution)));
		uint32_t maxMipLevels = mipmaps ? mipLevels : 1;

		uint32_t offset = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipResolution = (uint32_t)(this->props.resolution * std::pow(0.5, mip));

			for (uint32_t i = 0; i < 6; ++i)
			{
				if (compressed)
					MH_GL_CALL(glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, 0, 0, mipResolution, mipResolution, dataFormat, size, (char*)data + offset))
				else
					MH_GL_CALL(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, 0, 0, mipResolution, mipResolution, dataFormat, formatType, (char*)data + offset))

				offset += size;
			}

			size /= 4;
		}

		if (props.mipmaps && !mipmaps)
			MH_GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		MH_GL_CALL(glBindTextureUnit(slot, rendererID));
	}

	void OpenGLTextureCube::BindImage(uint32_t slot, bool read, bool write) const
	{
		uint32_t readWrite = (read && write) ? GL_READ_WRITE : (write ? GL_WRITE_ONLY : GL_READ_ONLY);

		glBindImageTexture(slot, rendererID, 0, GL_FALSE, 0, readWrite, internalFormat);
	}

	void OpenGLTextureCube::ReadPixels(void* pixels, bool mipmaps)
	{
		// TODO: Fix to work with compressed formats

		MH_PROFILE_FUNCTION();

		MH_GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID));

		uint32_t bpp = TextureFormatToByteSize(props.format);

		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(props.resolution))); // Should be +1, but it DOESN'T FUCKING WORK
		uint32_t maxMipLevels = mipmaps ? mipLevels : 1;

		uint32_t offset = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipResolution = (uint32_t)(props.resolution * std::pow(0.5, mip));
			uint32_t size = mipResolution * mipResolution * bpp;

			for (uint32_t i = 0; i < 6; ++i)
			{
				MH_GL_CALL(glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, dataFormat, formatType, (char*)pixels + offset));

				offset += size;
			}
		}


		/*MH_PROFILE_FUNCTION();

		uint32_t bpp = TextureFormatToByteSize(props.format);

		uint32_t mipLevels = 1 + (uint32_t)(std::floor(std::log2(std::max(props.width, props.height))));
		uint32_t maxMipLevels = mipmaps ? mipLevels : 1;

		uint32_t offset = 0;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			uint32_t mipSize;
			if (compressed)
			{
				int compressed_size;
				glGetTextureLevelParameteriv(rendererID, mip, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);

				mipSize = compressed_size;

				glGetCompressedTextureImage(rendererID, mip, mipSize, (char*)pixels + offset);
			}
			else
			{
				uint32_t mipWidth = (uint32_t)(this->props.width * std::pow(0.5, mip));
				uint32_t mipHeight = (uint32_t)(this->props.height * std::pow(0.5, mip));

				mipSize = mipWidth * mipHeight * bpp;
				glGetTextureImage(rendererID, mip, dataFormat, formatType, mipSize, (char*)pixels + offset);
			}

			offset += mipSize;
		}*/
	}
}