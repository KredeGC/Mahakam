#pragma once

#include "Mahakam/Renderer/Texture.h"

namespace Mahakam {
	class OpenGLTexture2D : public Texture2D
	{
	private:
		uint32_t rendererID;
		std::filesystem::path filepath;

		TextureProps m_Props;

		uint32_t internalFormat;
		uint32_t dataFormat;
		uint32_t formatType;

		bool compressed;
		uint32_t size;
		uint32_t totalSize;

	public:
		OpenGLTexture2D(const TextureProps& props);
		OpenGLTexture2D(const std::filesystem::path& filepath, const TextureProps& props);
		virtual ~OpenGLTexture2D();

		virtual const std::filesystem::path& GetFilepath() const override { return filepath; }

		virtual const TextureProps& GetProps() const override { return m_Props; }

		virtual uint32_t GetWidth() const override { return m_Props.width; }
		virtual uint32_t GetHeight() const override { return m_Props.height; }
		virtual uint32_t GetRendererID() const override { return rendererID; }

		virtual uint32_t GetSize() const override { return size; }
		virtual uint32_t GetTotalSize() const override { return totalSize; }

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
		uint32_t rendererID;
		std::filesystem::path filepath;
		uint32_t internalFormat;
		uint32_t dataFormat;
		uint32_t formatType;

		bool compressed;
		uint32_t size;
		uint32_t totalSize;

		CubeTextureProps props;

	public:
		OpenGLTextureCube(const CubeTextureProps& props);
		OpenGLTextureCube(const std::filesystem::path& filepath, const CubeTextureProps& props);
		OpenGLTextureCube(Asset<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props);
		virtual ~OpenGLTextureCube();

		virtual const std::filesystem::path& GetFilepath() const override { return filepath; }

		virtual const CubeTextureProps& GetProps() const override { return props; };

		virtual uint32_t GetWidth() const override { return props.resolution; }
		virtual uint32_t GetHeight() const override { return props.resolution; }
		virtual uint32_t GetRendererID() const override { return rendererID; }

		virtual uint32_t GetSize() const override { return size; }
		virtual uint32_t GetTotalSize() const override { return totalSize; }

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void SetData(void* data, uint32_t size, bool mipmaps) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual void BindImage(uint32_t slot = 0, bool read = true, bool write = true) const override;

		virtual void ReadPixels(void* pixels, bool mipmaps) override;
	};
}