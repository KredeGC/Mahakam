#pragma once

#include "Mahakam/Renderer/Texture.h"

namespace Mahakam {
	class OpenGLTexture2D;
	class OpenGLTextureCube;

	extern template class Asset<OpenGLTexture2D>;
	extern template class Asset<OpenGLTextureCube>;

	class OpenGLTexture2D : public Texture2D
	{
	private:
		uint32_t m_RendererID;
		std::filesystem::path m_Filepath;

		TextureProps m_Props;

		uint32_t m_InternalFormat;
		uint32_t m_DataFormat;
		uint32_t m_FormatType;

		bool m_Compressed;
		uint32_t m_Size;
		uint32_t m_TotalSize;

	public:
		OpenGLTexture2D(const TextureProps& props);
		OpenGLTexture2D(const std::filesystem::path& filepath, const TextureProps& props);
		virtual ~OpenGLTexture2D();

		virtual const std::filesystem::path& GetFilepath() const override { return m_Filepath; }

		virtual const TextureProps& GetProps() const override { return m_Props; }

		virtual uint32_t GetWidth() const override { return m_Props.Width; }
		virtual uint32_t GetHeight() const override { return m_Props.Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual uint32_t GetSize() const override { return m_Size; }
		virtual uint32_t GetTotalSize() const override { return m_TotalSize; }

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void SetData(void* data, uint32_t size, bool mipmaps) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual void BindImage(uint32_t slot = 0, bool read = true, bool write = true) const override;

		virtual void ReadPixels(void* pixels, bool mipmaps) override;

	private:
		void Init();
	};


	class OpenGLTextureCube : public TextureCube
	{
	private:
		uint32_t m_RendererID;
		std::filesystem::path m_Filepath;
		uint32_t m_InternalFormat;
		uint32_t m_DataFormat;
		uint32_t m_FormatType;

		bool m_Compressed;
		uint32_t m_Size;
		uint32_t m_TotalSize;

		CubeTextureProps m_Props;

	public:
		OpenGLTextureCube(const CubeTextureProps& props);
		OpenGLTextureCube(const std::filesystem::path& filepath, const CubeTextureProps& props);
		//OpenGLTextureCube(Asset<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props);
		virtual ~OpenGLTextureCube();

		virtual const std::filesystem::path& GetFilepath() const override { return m_Filepath; }

		virtual const CubeTextureProps& GetProps() const override { return m_Props; };

		virtual uint32_t GetWidth() const override { return m_Props.Resolution; }
		virtual uint32_t GetHeight() const override { return m_Props.Resolution; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual uint32_t GetSize() const override { return m_Size; }
		virtual uint32_t GetTotalSize() const override { return m_TotalSize; }

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void SetData(void* data, uint32_t size, bool mipmaps) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual void BindImage(uint32_t slot = 0, bool read = true, bool write = true) const override;

		virtual void ReadPixels(void* pixels, bool mipmaps) override;

	private:
		void CreateCubemap();
		void CreatePrefilter(uint32_t cubemapID);
	};
}