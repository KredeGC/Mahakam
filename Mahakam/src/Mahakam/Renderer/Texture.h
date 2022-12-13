#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include "RenderBuffer.h"
#include "TextureProps.h"

#include <filesystem>

namespace Mahakam
{
	class Texture : public RenderBuffer
	{
	public:
		virtual const std::filesystem::path& GetFilepath() const = 0;

		virtual uint32_t GetTotalSize() const = 0;

		virtual void SetData(void* data, uint32_t size, bool mipmaps = false) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual void BindImage(uint32_t slot = 0, bool read = true, bool write = true) const = 0;

		virtual void ReadPixels(void* pixels, bool mipmaps = false) = 0;

		virtual bool IsCubemap() const = 0;
	};


	class Texture2D : public Texture
	{
	public:
		virtual bool IsCubemap() const override { return false; }

		virtual const TextureProps& GetProps() const = 0;

		inline static Asset<Texture2D> Create(const TextureProps& props = TextureProps()) { return CreateProps(props); }
		inline static Asset<Texture2D> Create(const std::filesystem::path& filepath, const TextureProps& props = TextureProps()) { return CreateFilepath(filepath, props); }

	private:
		MH_DECLARE_FUNC(CreateProps, Asset<Texture2D>, const TextureProps& props);
		MH_DECLARE_FUNC(CreateFilepath, Asset<Texture2D>, const std::filesystem::path& filepath, const TextureProps& props);
	};


	class Texture2DArray : public Texture
	{
	public:
		virtual bool IsCubemap() const override { return false; }

		static Asset<Texture2DArray> Create(const TextureProps& props = TextureProps());
	};


	class Texture3D : public Texture
	{
	public:
		virtual bool IsCubemap() const override { return false; }

		virtual uint32_t GetDepth() const = 0;

		static Asset<Texture3D> Create(const std::filesystem::path& filepath, const TextureProps& props = TextureProps());
	};


	class TextureCube : public Texture
	{
	public:
		virtual bool IsCubemap() const override { return true; }

		virtual const CubeTextureProps& GetProps() const = 0;

		inline static Asset<TextureCube> Create(const CubeTextureProps& props = CubeTextureProps()) { return CreateProps(props); }
		inline static Asset<TextureCube> Create(const std::filesystem::path& filepath, const CubeTextureProps& props = CubeTextureProps()) { return CreateFilepath(filepath, props); }
		
	private:
		MH_DECLARE_FUNC(CreateProps, Asset<TextureCube>, const CubeTextureProps& props);
		MH_DECLARE_FUNC(CreateFilepath, Asset<TextureCube>, const std::filesystem::path& filepath, const CubeTextureProps& props);
	};
}