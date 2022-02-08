#pragma once

#include "Mahakam/Core/Core.h"
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
		bool mipmaps;

		CubeTextureProps(uint32_t resolution = 1024, TextureFormat format = TextureFormat::RGB8, TextureFilter filterMode = TextureFilter::Trilinear, bool mipmaps = true)
			: resolution(resolution), format(format), filterMode(filterMode), mipmaps(mipmaps) {}

		CubeTextureProps(uint32_t resolution, TextureFormat format, bool mipmaps)
			: resolution(resolution), format(format), filterMode(TextureFilter::Trilinear), mipmaps(mipmaps) {}
	};


	class Texture : public RenderBuffer
	{
	public:
		virtual uint32_t GetTotalSize() const = 0;

		virtual void SetData(void* data, uint32_t size, bool mipmaps = false) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual void ReadPixels(void* pixels, bool mipmaps = false) = 0;
	};


	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> white;
		static Ref<Texture2D> black;
		static Ref<Texture2D> bump;

		static Ref<Texture2D> Create(const TextureProps& props = TextureProps());
		static Ref<Texture2D> Create(const std::string& filepath, const TextureProps& props = TextureProps());
	};


	class Texture2DArray : public Texture
	{
	public:
		static Ref<Texture2DArray> Create(const TextureProps& props = TextureProps());
	};


	class Texture3D : public Texture
	{
	public:
		virtual uint32_t GetDepth() const = 0;

		static Ref<Texture3D> Create(const std::string& filepath, const TextureProps& props = TextureProps());
	};


	class TextureCube : public Texture
	{
	private:
		uint32_t resolution;

	public:
		static Ref<TextureCube> white;

		static Ref<TextureCube> Create(const CubeTextureProps& props = CubeTextureProps());
		static Ref<TextureCube> Create(const std::string& filepath, const CubeTextureProps& props = CubeTextureProps());
		static Ref<TextureCube> Create(Ref<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props = CubeTextureProps());
	};
}