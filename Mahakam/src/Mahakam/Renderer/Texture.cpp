#include "Mahakam/mhpch.h"
#include "Texture.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Mahakam
{
	//Ref<Texture2D> Texture2D::Create(const TextureProps& props)
	MH_DEFINE_FUNC(Texture2D::CreateProps, Ref<Texture2D>, const TextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Ref<Texture2D> Texture2D::Create(const std::string& filepath, const TextureProps& props)
	MH_DEFINE_FUNC(Texture2D::CreateFilepath, Ref<Texture2D>, const std::filesystem::path& filepath, const TextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(filepath, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};


	//Ref<TextureCube> TextureCube::Create(const CubeTextureProps& props)
	MH_DEFINE_FUNC(TextureCube::CreateProps, Ref<TextureCube>, const CubeTextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Ref<TextureCube> TextureCube::Create(const std::string& filepath, const CubeTextureProps& props)
	MH_DEFINE_FUNC(TextureCube::CreateFilepath, Ref<TextureCube>, const std::filesystem::path& filepath, const CubeTextureProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(filepath, props);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}