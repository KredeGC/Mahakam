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
		OpenGLTexture2D(const std::string& filepath);
		virtual ~OpenGLTexture2D();

		virtual uint32_t getWidth() const { return width; }
		virtual uint32_t getHeight() const { return height; }

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
		OpenGLTextureCube(const std::vector<std::string>& faces);
		OpenGLTextureCube(const std::string& filepath);
		virtual ~OpenGLTextureCube();

		virtual uint32_t getWidth() const { return width; }
		virtual uint32_t getHeight() const { return height; }

		virtual void bind(uint32_t slot = 0) const override;
	};
}