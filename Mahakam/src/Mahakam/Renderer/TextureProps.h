#pragma once

#include "TextureFormats.h"

#include <cstdint>

namespace Mahakam
{
	struct TextureProps
	{
		uint32_t width;
		uint32_t height;
		TextureFormat format;
		TextureFilter filterMode;
		TextureWrapMode wrapX;
		TextureWrapMode wrapY;
		bool mipmaps;

		TextureProps(TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Bilinear,
			TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true)
			: width(0), height(0), format(format), filterMode(filterMode), wrapX(wrapX), wrapY(wrapY), mipmaps(mipmaps) {}

		TextureProps(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Bilinear,
			TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true)
			: width(width), height(height), format(format), filterMode(filterMode), wrapX(wrapX), wrapY(wrapY), mipmaps(mipmaps) {}
	};

	struct CubeTextureProps
	{
		uint32_t resolution;
		TextureFormat format;
		TextureFilter filterMode;
		TextureCubePrefilter prefilter;
		bool mipmaps;

		CubeTextureProps(uint32_t resolution = 1024, TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Trilinear, TextureCubePrefilter prefilter = TextureCubePrefilter::None, bool mipmaps = true)
			: resolution(resolution), format(format), filterMode(filterMode), prefilter(prefilter), mipmaps(mipmaps) {}

		CubeTextureProps(uint32_t resolution, TextureFormat format, TextureCubePrefilter prefilter, bool mipmaps)
			: resolution(resolution), format(format), filterMode(TextureFilter::Trilinear), prefilter(prefilter), mipmaps(mipmaps) {}
	};
}