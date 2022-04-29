#include "mhpch.h"
#include "ComputeShader.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLComputeShader.h"

namespace Mahakam
{
	//Ref<ComputeShader> ComputeShader::Create(const std::string& filepath)
	MH_DEFINE_FUNC(ComputeShader::Create, Ref<ComputeShader>, const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLComputeShader>(filepath);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}