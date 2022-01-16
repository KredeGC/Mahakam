#pragma once

#include "Mahakam/Renderer/Texture.h"

namespace Mahakam {
	class OpenGLTexture2D : public Texture2D
	{
	private:
		uint32_t rendererID;
		std::string filepath;
		uint32_t internalFormat;
		uint32_t dataFormat;
		uint32_t formatType;

		TextureProps props;

	public:
		OpenGLTexture2D(const TextureProps& props);
		OpenGLTexture2D(const std::string& filepath, const TextureProps& props);
		virtual ~OpenGLTexture2D();

		virtual uint32_t getWidth() const override { return props.width; }
		virtual uint32_t getHeight() const override { return props.height; }
		virtual uint32_t getRendererID() const override { return rendererID; }

		virtual void setData(void* data, bool mipmaps) override;

		virtual void bind(uint32_t slot = 0) const override;

		virtual void readPixels(void* pixels, bool mipmaps) override;
	};


	class OpenGLTextureCube : public TextureCube
	{
	private:
		uint32_t rendererID;
		std::string filepath;
		uint32_t internalFormat;
		uint32_t dataFormat;
		uint32_t formatType;

		CubeTextureProps props;

	public:
		OpenGLTextureCube(const CubeTextureProps& props);
		OpenGLTextureCube(const std::vector<std::string>& faces, const CubeTextureProps& props);
		OpenGLTextureCube(const std::string& filepath, const CubeTextureProps& props);
		virtual ~OpenGLTextureCube();

		virtual uint32_t getWidth() const override { return props.resolution; }
		virtual uint32_t getHeight() const override { return props.resolution; }
		virtual uint32_t getRendererID() const override { return rendererID; }

		virtual void setData(void* data, bool mipmaps) override;

		virtual void bind(uint32_t slot = 0) const override;

		virtual void readPixels(void* pixels, bool mipmaps) override;
	};
}