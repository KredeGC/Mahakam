#pragma once

#include "TextureFormats.h"

#include <cstdint>

namespace Mahakam
{
	struct TextureProps
	{
		uint32_t Width;
		uint32_t Height;
		TextureFormat Format;
		TextureFilter FilterMode;
		TextureWrapMode WrapX;
		TextureWrapMode WrapY;
		bool Mipmaps;

		TextureProps(TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Bilinear,
			TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true)
			: Width(0), Height(0), Format(format), FilterMode(filterMode), WrapX(wrapX), WrapY(wrapY), Mipmaps(mipmaps) {}

		TextureProps(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Bilinear,
			TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true)
			: Width(width), Height(height), Format(format), FilterMode(filterMode), WrapX(wrapX), WrapY(wrapY), Mipmaps(mipmaps) {}
	};

	struct CubeTextureProps
	{
		uint32_t Resolution;
		TextureFormat Format;
		TextureFilter FilterMode;
		TextureCubePrefilter Prefilter;
		bool Mipmaps;

		CubeTextureProps(uint32_t resolution = 1024, TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Trilinear, TextureCubePrefilter prefilter = TextureCubePrefilter::None, bool mipmaps = true)
			: Resolution(resolution), Format(format), FilterMode(filterMode), Prefilter(prefilter), Mipmaps(mipmaps) {}

		CubeTextureProps(uint32_t resolution, TextureFormat format, TextureCubePrefilter prefilter, bool mipmaps)
			: Resolution(resolution), Format(format), FilterMode(TextureFilter::Trilinear), Prefilter(prefilter), Mipmaps(mipmaps) {}
	};
}