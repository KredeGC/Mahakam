#include "mhpch.h"
#include "ComputeShader.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLComputeShader.h"

namespace Mahakam
{
	Ref<ComputeShader> ComputeShader::Create(const std::string& filepath, uint32_t x, uint32_t y, uint32_t z)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLComputeShader>(filepath, x, y, z);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}