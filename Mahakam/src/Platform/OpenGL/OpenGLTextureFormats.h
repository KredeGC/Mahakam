#pragma once
#include "Mahakam/Core/Log.h"
#include "Mahakam/Renderer/TextureFormats.h"

#include <glad/glad.h>

// ONLY INCLUDE IN CPP FILES

namespace Mahakam
{
	static uint32_t TextureFormatToByteSize(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8:
			return 1;
		case TextureFormat::RG8:
			return 2;
		case TextureFormat::RGB8:
			return 3;
		case TextureFormat::RGBA8:
			return 4;
		case TextureFormat::R16F:
			return 2;
		case TextureFormat::RG16F:
			return 4;
		case TextureFormat::RGB16F:
			return 6;
		case TextureFormat::RGBA16F:
			return 8;
		case TextureFormat::R32F:
			return 4;
		case TextureFormat::RG32F:
			return 8;
		case TextureFormat::RGB32F:
			return 12;
		case TextureFormat::RGBA32F:
			return 16;
		case TextureFormat::RG11B10F:
			return 4;
		case TextureFormat::RGB10A2:
			return 4;
		case TextureFormat::Depth16:
			return 2;
		case TextureFormat::Depth24:
			return 3;
		case TextureFormat::Depth24Stencil8:
			return 4;
		}

		MH_CORE_BREAK("Unknown TextureFormat provided!");
		return 0;
	}

	static GLenum TextureFormatToOpenGLFormat(TextureFormat format, int channels)
	{
		uint32_t component = 0;
		switch (channels)
		{
		case 1:
			component = GL_RED;
			break;
		case 2:
			component = GL_RG;
			break;
		case 3:
			component = GL_RGB;
			break;
		case 4:
			component = GL_RGBA;
			break;
		default:
			MH_CORE_BREAK("Unsupported number of channels!");
		}

		switch (format)
		{
		case TextureFormat::R8:
			return component;
		case TextureFormat::RG8:
			return component;
		case TextureFormat::RGB8:
			return component;
		case TextureFormat::RGBA8:
			return component;
		case TextureFormat::R16F:
			return component;
		case TextureFormat::RG16F:
			return component;
		case TextureFormat::RGB16F:
			return component;
		case TextureFormat::RGBA16F:
			return component;
		case TextureFormat::R32F:
			return component;
		case TextureFormat::RG32F:
			return component;
		case TextureFormat::RGB32F:
			return component;
		case TextureFormat::RGBA32F:
			return component;
		case TextureFormat::RG11B10F:
			return GL_R11F_G11F_B10F;
		case TextureFormat::RGB10A2:
			return GL_RGB10_A2;
		case TextureFormat::Depth16:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::Depth24:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::Depth24Stencil8:
			return GL_DEPTH_STENCIL;
		}

		MH_CORE_BREAK("Unknown TextureFormat provided!");
		return 0;
	}

	static GLenum TextureFormatToOpenGLFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8:
			return GL_RED;
		case TextureFormat::RG8:
			return GL_RG;
		case TextureFormat::RGB8:
			return GL_RGB;
		case TextureFormat::RGBA8:
			return GL_RGBA;
		case TextureFormat::R16F:
			return GL_RED;
		case TextureFormat::RG16F:
			return GL_RG;
		case TextureFormat::RGB16F:
			return GL_RGB;
		case TextureFormat::RGBA16F:
			return GL_RGBA;
		case TextureFormat::R32F:
			return GL_RED;
		case TextureFormat::RG32F:
			return GL_RG;
		case TextureFormat::RGB32F:
			return GL_RGB;
		case TextureFormat::RGBA32F:
			return GL_RGBA;
		case TextureFormat::RG11B10F:
			return GL_R11F_G11F_B10F;
		case TextureFormat::RGB10A2:
			return GL_RGB10_A2;
		case TextureFormat::Depth16:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::Depth24:
			return GL_DEPTH_COMPONENT;
		case TextureFormat::Depth24Stencil8:
			return GL_DEPTH_STENCIL;
		}

		MH_CORE_BREAK("Unknown TextureFormat provided!");
		return 0;
	}

	static GLenum TextureFormatToOpenGLInternalFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8:
			return GL_R8;
		case TextureFormat::RG8:
			return GL_RG8;
		case TextureFormat::RGB8:
			return GL_RGB8;
		case TextureFormat::RGBA8:
			return GL_RGBA8;
		case TextureFormat::R16F:
			return GL_R16F;
		case TextureFormat::RG16F:
			return GL_RG16F;
		case TextureFormat::RGB16F:
			return GL_RGB16F;
		case TextureFormat::RGBA16F:
			return GL_RGBA16F;
		case TextureFormat::R32F:
			return GL_R32F;
		case TextureFormat::RG32F:
			return GL_RG32F;
		case TextureFormat::RGB32F:
			return GL_RGB32F;
		case TextureFormat::RGBA32F:
			return GL_RGBA32F;
		case TextureFormat::RG11B10F:
			return GL_R11F_G11F_B10F;
		case TextureFormat::RGB10A2:
			return GL_RGB10_A2;
		case TextureFormat::Depth16:
			return GL_DEPTH_COMPONENT16;
		case TextureFormat::Depth24:
			return GL_DEPTH_COMPONENT24;
		case TextureFormat::Depth24Stencil8:
			return GL_DEPTH24_STENCIL8;
		}

		MH_CORE_BREAK("Unknown TextureFormat provided!");
		return 0;
	}

	static GLenum TextureFormatToOpenGLType(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RG8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RGB8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::RGBA8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::R16F:
			return GL_HALF_FLOAT;
		case TextureFormat::RG16F:
			return GL_HALF_FLOAT;
		case TextureFormat::RGB16F:
			return GL_HALF_FLOAT;
		case TextureFormat::RGBA16F:
			return GL_HALF_FLOAT;
		case TextureFormat::R32F:
			return GL_FLOAT;
		case TextureFormat::RG32F:
			return GL_FLOAT;
		case TextureFormat::RGB32F:
			return GL_FLOAT;
		case TextureFormat::RGBA32F:
			return GL_FLOAT;
		case TextureFormat::RG11B10F:
			return GL_FLOAT;
		case TextureFormat::RGB10A2:
			return GL_UNSIGNED_INT_10_10_10_2;
		case TextureFormat::Depth16:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::Depth24:
			return GL_UNSIGNED_INT;
		case TextureFormat::Depth24Stencil8:
			return GL_UNSIGNED_INT_24_8;
		}

		MH_CORE_BREAK("Unknown TextureFormat provided!");
		return 0;
	}

	static GLenum TextureFormatToOpenGLAttachment(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::Depth16:
			return GL_DEPTH_ATTACHMENT;
		case TextureFormat::Depth24:
			return GL_DEPTH_ATTACHMENT;
		case TextureFormat::Depth24Stencil8:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		}

		MH_CORE_BREAK("Unknown TextureFormat provided!");
		return 0;
	}
}