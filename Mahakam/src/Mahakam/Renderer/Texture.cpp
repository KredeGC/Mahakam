#include "mhpch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Mahakam
{
	Ref<Texture2D> Texture2D::Create(const TextureProps& props)
	{
		MH_OVERRIDE_FUNC(Texture2DCreateProps, props);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filepath, const TextureProps& props)
	{
		MH_OVERRIDE_FUNC(Texture2DCreateFilepath, filepath, props);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(filepath, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}


	Ref<TextureCube> TextureCube::Create(const CubeTextureProps& props)
	{
		MH_OVERRIDE_FUNC(TextureCubeCreateProps, props);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(const std::string& filepath, const CubeTextureProps& props)
	{
		MH_OVERRIDE_FUNC(TextureCubeCreateFilepath, filepath, props);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(filepath, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(Ref<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	{
		MH_OVERRIDE_FUNC(TextureCubeCreatePrefilter, cubemap, prefilter, props);

		switch (RendererAPI::GetAPI())
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