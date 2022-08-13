#include "Mahakam/mhpch.h"
#include "Material.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLMaterial.h"

namespace Mahakam
{
	//Ref<Material> Material::CreateImpl(Ref<Shader> shader, const std::string& variant)
	MH_DEFINE_FUNC(Material::CreateImpl, Asset<Material>, Asset<Shader> shader, const std::string& variant)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLMaterial>(CreateRef<OpenGLMaterial>(shader, variant));
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Ref<Material> Material::Copy(Ref<Material> material)
	MH_DEFINE_FUNC(Material::Copy, Asset<Material>, Asset<Material> material)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLMaterial>(CreateRef<OpenGLMaterial>(material));
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}