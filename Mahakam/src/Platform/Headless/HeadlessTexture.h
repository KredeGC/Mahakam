#pragma once

#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	class HeadlessTexture2D;
	class HeadlessTextureCube;

	extern template class Asset<HeadlessTexture2D>;
	extern template class Asset<HeadlessTextureCube>;

	class HeadlessTexture2D : public Texture2D
	{
	private:
		std::filesystem::path m_Filepath;

		TextureProps m_Props;

		uint32_t m_Size;
		uint32_t m_TotalSize;

	public:
		HeadlessTexture2D(const TextureProps& props);
		HeadlessTexture2D(const std::filesystem::path& filepath, const TextureProps& props);
		virtual ~HeadlessTexture2D();

		virtual const std::filesystem::path& GetFilepath() const override { return m_Filepath; }

		virtual const TextureProps& GetProps() const override { return m_Props; }

		virtual uint32_t GetWidth() const override { return m_Props.Width; }
		virtual uint32_t GetHeight() const override { return m_Props.Height; }
		virtual uint32_t GetRendererID() const override { return 0; }

		virtual uint32_t GetSize() const override { return m_Size; }
		virtual uint32_t GetTotalSize() const override { return m_TotalSize; }

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void SetData(void* data, uint32_t size, bool mipmaps) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual void BindImage(uint32_t slot = 0, bool read = true, bool write = true) const override;

		virtual void ReadPixels(void* pixels, bool mipmaps) override;
	};


	class HeadlessTextureCube : public TextureCube
	{
	private:
		std::filesystem::path m_Filepath;

		CubeTextureProps m_Props;

		uint32_t m_Size;
		uint32_t m_TotalSize;

	public:
		HeadlessTextureCube(const CubeTextureProps& props);
		HeadlessTextureCube(const std::filesystem::path& filepath, const CubeTextureProps& props);
		virtual ~HeadlessTextureCube();

		virtual const std::filesystem::path& GetFilepath() const override { return m_Filepath; }

		virtual const CubeTextureProps& GetProps() const override { return m_Props; };

		virtual uint32_t GetWidth() const override { return m_Props.Resolution; }
		virtual uint32_t GetHeight() const override { return m_Props.Resolution; }
		virtual uint32_t GetRendererID() const override { return 0; }

		virtual uint32_t GetSize() const override { return m_Size; }
		virtual uint32_t GetTotalSize() const override { return m_TotalSize; }

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void SetData(void* data, uint32_t size, bool mipmaps) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual void BindImage(uint32_t slot = 0, bool read = true, bool write = true) const override;

		virtual void ReadPixels(void* pixels, bool mipmaps) override;
	};
}