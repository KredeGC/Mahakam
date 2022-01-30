#include "mhpch.h"
#include "Material.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLMaterial.h"

namespace Mahakam
{
	Ref<Material> Material::copy(Ref<Material> material)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLMaterial>(material);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

	Ref<Material> Material::create(Ref<Shader> shader, const std::string& variant)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLMaterial>(shader, variant);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}