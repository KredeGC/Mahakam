#include "mhpch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Mahakam
{
	//Ref<Texture2D> Texture2D::Create(const TextureProps& props)
	MH_DEFINE_FUNC(Texture2D::CreateProps, Asset<Texture2D>, const TextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLTexture2D>::Create(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Ref<Texture2D> Texture2D::Create(const std::string& filepath, const TextureProps& props)
	MH_DEFINE_FUNC(Texture2D::CreateFilepath, Asset<Texture2D>, const std::string& filepath, const TextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLTexture2D>::Create(filepath, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};


	//Ref<TextureCube> TextureCube::Create(const CubeTextureProps& props)
	MH_DEFINE_FUNC(TextureCube::CreateProps, Asset<TextureCube>, const CubeTextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLTextureCube>::Create(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Ref<TextureCube> TextureCube::Create(const std::string& filepath, const CubeTextureProps& props)
	MH_DEFINE_FUNC(TextureCube::CreateFilepath, Asset<TextureCube>, const std::string& filepath, const CubeTextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLTextureCube>::Create(filepath, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Ref<TextureCube> TextureCube::Create(Ref<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	MH_DEFINE_FUNC(TextureCube::CreatePrefilter, Asset<TextureCube>, Asset<TextureCube> cubemap, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLTextureCube>::Create(cubemap, prefilter, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}