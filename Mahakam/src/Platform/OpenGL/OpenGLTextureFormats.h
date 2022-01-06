#pragma once
#include "Mahakam/Core/Log.h"
#include "Mahakam/Renderer/Texture.h"

#include <glad/glad.h>

// ONLY INCLUDE IN CPP FILES

namespace Mahakam
{
	static GLenum TextureFormatToOpenGLFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RG8:
			return GL_RG;
		case TextureFormat::RGB8:
			return GL_RGB;
		case TextureFormat::RGBA8:
			return GL_RGBA;
		case TextureFormat::RG16F:
			return GL_RG;
		case TextureFormat::RGBA16F:
			return GL_RGB;
		case TextureFormat::Depth24:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::Depth24Stencil8:
			return GL_DEPTH_STENCIL;
		default:
			MH_CORE_ASSERT(false, "Unknown TextureFormat provided!");
		}

		return 0;
	}

	static GLenum TextureFormatToOpenGLInternalFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RG8:
			return GL_RG8;
		case TextureFormat::RGB8:
			return GL_RGB8;
		case TextureFormat::RGBA8:
			return GL_RGBA8;
		case TextureFormat::RG16F:
			return GL_RG16F;
		case TextureFormat::RGBA16F:
			return GL_RGBA16F;
		case TextureFormat::Depth24:
			return GL_DEPTH_COMPONENT24;
		case TextureFormat::Depth24Stencil8:
			return GL_DEPTH24_STENCIL8;
		default:
			MH_CORE_ASSERT(false, "Unknown TextureFormat provided!");
		}

		return 0;
	}

	static GLenum TextureFormatToOpenGLType(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RG8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RGB8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RGBA8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RG16F:
			return GL_FLOAT;
		case TextureFormat::RGBA16F:
			return GL_FLOAT;
		case TextureFormat::Depth24:
			return GL_UNSIGNED_INT; // Int24?
		case TextureFormat::Depth24Stencil8:
			return GL_UNSIGNED_INT_24_8;
		default:
			MH_CORE_ASSERT(false, "Unknown TextureFormat provided!");
		}

		return 0;
	}

	static GLenum TextureFormatToOpenGLAttachment(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RG8:
			return GL_COLOR_ATTACHMENT0;
		case TextureFormat::RGB8:
			return GL_COLOR_ATTACHMENT0;
		case TextureFormat::RGBA8:
			return GL_COLOR_ATTACHMENT0;
		case TextureFormat::RG16F:
			return GL_COLOR_ATTACHMENT0;
		case TextureFormat::RGBA16F:
			return GL_COLOR_ATTACHMENT0;
		case TextureFormat::Depth24:
			return GL_DEPTH_ATTACHMENT;
		case TextureFormat::Depth24Stencil8:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		default:
			MH_CORE_ASSERT(false, "Unknown TextureFormat provided!");
		}

		return 0;
	}
}