#pragma once

#include "Mahakam/Core/Core.h"

#include <string>

namespace Mahakam
{
	enum class TextureFilter
	{
		Point = 0,
		Bilinear
	};

	enum class TextureWrapMode
	{
		Clamp = 0,
		Repeat
	};

	struct TextureProps
	{
		TextureFilter filterMode;
		TextureWrapMode wrapX;
		TextureWrapMode wrapY;
		bool mipmaps;

		TextureProps(TextureFilter filterMode = TextureFilter::Bilinear, TextureWrapMode wrapX = TextureWrapMode::Repeat, TextureWrapMode wrapY = TextureWrapMode::Repeat, bool mipmaps = true)
			: filterMode(filterMode), wrapX(wrapX), wrapY(wrapY), mipmaps(mipmaps) {}
	};


	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getRendererID() const = 0;

		virtual void bind(uint32_t slot = 0) const = 0;
	};


	class Texture2D : public Texture
	{
	public:
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
		static Ref<TextureCube> create(const std::string& filepath, const TextureProps& props = TextureProps());
	};
}