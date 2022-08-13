#include "Mahakam/mhpch.h"
#include "ComputeShader.h"

#include "RendererAPI.h"
#include "Mahakam/Asset/Asset.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLComputeShader.h"

namespace Mahakam
{
	//Ref<ComputeShader> ComputeShader::Create(const std::string& filepath)
	MH_DEFINE_FUNC(ComputeShader::Create, Asset<ComputeShader>, const std::filesystem::path& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLComputeShader>(CreateRef<OpenGLComputeShader>(filepath));
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}