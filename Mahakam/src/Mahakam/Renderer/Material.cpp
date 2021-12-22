#include "mhpch.h"
#include "Material.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLMaterial.h"

namespace Mahakam
{
	Ref<Material> Material::create(Ref<Shader> shader)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLMaterial>(shader);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}
}