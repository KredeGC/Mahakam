#include "mhpch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Mahakam
{
	Ref<Texture2D> Texture2D::white;
	Ref<Texture2D> Texture2D::black;
	Ref<Texture2D> Texture2D::bump;
	Ref<TextureCube> TextureCube::white;

	Ref<Texture2D> Texture2D::create(const TextureProps& props)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

	Ref<Texture2D> Texture2D::create(const std::string& filepath, bool sRGB, const TextureProps& props)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(filepath, sRGB, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}


	Ref<TextureCube> TextureCube::create(const CubeTextureProps& props)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

	Ref<TextureCube> TextureCube::create(const std::string& filepath, const CubeTextureProps& props)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(filepath, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

	Ref<TextureCube> TextureCube::create(Ref<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(cubemap, prefilter, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}