#include "mhpch.h"
#include "Texture.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Mahakam
{
	Ref<Texture2D> Texture2D::create(const std::string& filepath)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLTexture2D>(filepath);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}


	Ref<TextureCube> TextureCube::create(const std::string& filepath)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLTextureCube>(filepath);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}
}