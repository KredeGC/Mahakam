#pragma once

#include "Mahakam/Renderer/Texture.h"

namespace Mahakam {
	class OpenGLTexture2D : public Texture2D
	{
	private:
		uint32_t rendererID;
		uint32_t width, height;
		std::string filepath;

	public:
		OpenGLTexture2D(const std::string& filepath, const TextureProps& props);
		virtual ~OpenGLTexture2D();

		virtual uint32_t getWidth() const override { return width; }
		virtual uint32_t getHeight() const override { return height; }
		virtual uint32_t getRendererID() const override { return rendererID; }

		virtual void bind(uint32_t slot = 0) const override;
	};


	class OpenGLTextureCube : public TextureCube
	{
	private:
		uint32_t rendererID;
		uint32_t width, height;
		std::string filepath;

		uint32_t captureFBO;
		uint32_t captureRBO;

	public:
		OpenGLTextureCube(const std::vector<std::string>& faces, const TextureProps& props);
		OpenGLTextureCube(const std::string& filepath, const TextureProps& props);
		virtual ~OpenGLTextureCube();

		virtual uint32_t getWidth() const override { return width; }
		virtual uint32_t getHeight() const override { return height; }
		virtual uint32_t getRendererID() const override { return rendererID; }

		virtual void bind(uint32_t slot = 0) const override;
	};
}