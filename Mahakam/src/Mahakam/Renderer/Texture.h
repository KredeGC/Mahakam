#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Asset/Asset.h"
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

		virtual void BindImage(uint32_t slot = 0, bool read = true, bool write = true) const = 0;

		virtual void ReadPixels(void* pixels, bool mipmaps = false) = 0;
	};


	class Texture2D : public Texture
	{
	public:
		virtual const TextureProps& GetProps() const = 0;

		inline static Asset<Texture2D> Create(const TextureProps& props = TextureProps()) { return CreateProps(props); }
		inline static Asset<Texture2D> Create(const std::string& filepath, const TextureProps& props = TextureProps()) { return CreateFilepath(filepath, props); }

	private:
		MH_DECLARE_FUNC(CreateProps, Asset<Texture2D>, const TextureProps& props);
		MH_DECLARE_FUNC(CreateFilepath, Asset<Texture2D>, const std::string& filepath, const TextureProps& props);
	};


	class Texture2DArray : public Texture
	{
	public:
		static Asset<Texture2DArray> Create(const TextureProps& props = TextureProps());
	};


	class Texture3D : public Texture
	{
	public:
		virtual uint32_t GetDepth() const = 0;

		static Asset<Texture3D> Create(const std::string& filepath, const TextureProps& props = TextureProps());
	};


	class TextureCube : public Texture
	{
	public:
		virtual const CubeTextureProps& GetProps() const = 0;

		inline static Asset<TextureCube> Create(const CubeTextureProps& props = CubeTextureProps()) { return CreateProps(props); }
		inline static Asset<TextureCube> Create(const std::string& filepath, const CubeTextureProps& props = CubeTextureProps()) { return CreateFilepath(filepath, props); }
		inline static Asset<TextureCube> Create(Asset<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props = CubeTextureProps()) { return CreatePrefilter(cubemap, prefilter, props); }

	private:
		MH_DECLARE_FUNC(CreateProps, Asset<TextureCube>, const CubeTextureProps& props);
		MH_DECLARE_FUNC(CreateFilepath, Asset<TextureCube>, const std::string& filepath, const CubeTextureProps& props);
		MH_DECLARE_FUNC(CreatePrefilter, Asset<TextureCube>, Asset<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps&);
	};
}