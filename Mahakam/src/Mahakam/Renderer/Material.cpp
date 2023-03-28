#include "Mahakam/mhpch.h"
#include "Material.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/Headless/HeadlessMaterial.h"
#include "Platform/OpenGL/OpenGLMaterial.h"

namespace Mahakam
{
	//Ref<Material> Material::CreateImpl(Ref<Shader> shader, const std::string& variant)
	MH_DEFINE_FUNC(Material::CreateImpl, Asset<Material>, Asset<Shader> shader)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateAsset<HeadlessMaterial>(shader);
		case RendererAPI::API::OpenGL:
			return CreateAsset<OpenGLMaterial>(shader);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};

	//Ref<Material> Material::Copy(Ref<Material> material)
	MH_DEFINE_FUNC(Material::Copy, Asset<Material>, Asset<Material> material)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateAsset<HeadlessMaterial>(*static_cast<HeadlessMaterial*>(material.get()));
		case RendererAPI::API::OpenGL:
			return CreateAsset<OpenGLMaterial>(*static_cast<OpenGLMaterial*>(material.get()));
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};
}