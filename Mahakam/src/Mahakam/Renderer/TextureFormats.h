#pragma once

namespace Mahakam
{
	enum class TextureFormat
	{
		RG8 = 0,
		RGB8,
		RGBA8,
		RG16F,
		RGBA16F,
		Depth24,
		Depth24Stencil8
	};

	enum class TextureFilter
	{
		Point = 0,
		Bilinear,
		Trilinear
	};

	enum class TextureWrapMode
	{
		Clamp = 0,
		Repeat
	};

	enum class TextureCubePrefilter
	{
		None = 0,
		Convolute,
		Prefilter
	};
}