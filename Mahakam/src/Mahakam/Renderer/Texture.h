#pragma once

#include "Mahakam/Core/Core.h"
#include "FrameBuffer.h"
#include "TextureFormats.h"
#include "RenderBuffer.h"

#include <string>

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

		TextureProps(TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Bilinear, TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true)
			: width(1024), height(1024), format(format), filterMode(filterMode), wrapX(wrapX), wrapY(wrapY), mipmaps(mipmaps) {}

		TextureProps(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Bilinear, TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true)
			: width(width), height(height), format(format), filterMode(filterMode), wrapX(wrapX), wrapY(wrapY), mipmaps(mipmaps) {}
	};

	struct CubeTextureProps
	{
		uint32_t resolution;
		TextureFormat format;
		TextureFilter filterMode;
		TextureWrapMode wrapX;
		TextureWrapMode wrapY;
		bool mipmaps;
		TextureCubePrefilter prefilter;

		CubeTextureProps(uint32_t resolution = 1024, TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Bilinear, TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true, TextureCubePrefilter prefilter = TextureCubePrefilter::None)
			: resolution(resolution), format(format), filterMode(filterMode), wrapX(wrapX), wrapY(wrapY), mipmaps(mipmaps), prefilter(prefilter) {}

		CubeTextureProps(uint32_t resolution, TextureFormat format, bool mipmaps, TextureCubePrefilter prefilter)
			: resolution(resolution), format(format), filterMode(TextureFilter::Bilinear), wrapX(TextureWrapMode::Repeat), wrapY(TextureWrapMode::Repeat), mipmaps(mipmaps), prefilter(prefilter) {}
	};


	class Texture : public RenderBuffer
	{
	public:
		virtual void setData(void* data, uint32_t size) = 0;

		virtual void bind(uint32_t slot = 0) const = 0;
	};


	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> create(uint32_t rendererID, const TextureProps& props = TextureProps());
		static Ref<Texture2D> create(const TextureProps& props = TextureProps());
		static Ref<Texture2D> create(const std::string& filepath, const TextureProps& props = TextureProps());
	};


	class Texture3D : public Texture
	{
	public:
		virtual uint32_t getDepth() const = 0;

		static Ref<Texture3D> create(const std::string& filepath, const TextureProps& props = TextureProps());
	};


	class TextureCube : public Texture
	{
	private:
		uint32_t resolution;

	public:
		static Ref<TextureCube> create(const std::string& filepath, const CubeTextureProps& props = CubeTextureProps());
	};
}