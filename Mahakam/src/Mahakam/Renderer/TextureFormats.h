#pragma once

namespace Mahakam
{
	enum class TextureFormat
	{
		R8 = 0,
		RG8,
		RGB8,
		RGBA8,
		R16F,
		RG16F,
		RGB16F,
		RGBA16F,
		R32F,
		RG32F,
		RGB32F,
		RGBA32F,
		RG11B10F,
		RGB10A2,
		Depth16,
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
		Repeat = 0,
		Clamp,
		ClampBorder
	};

	enum class TextureCubePrefilter
	{
		Convolute = 0,
		Prefilter
	};
}