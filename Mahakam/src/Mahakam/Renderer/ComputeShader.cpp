#include "Mahakam/mhpch.h"
#include "ComputeShader.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/Headless/HeadlessComputeShader.h"
#include "Platform/OpenGL/OpenGLComputeShader.h"

namespace Mahakam
{
	//Ref<ComputeShader> ComputeShader::Create(const std::string& filepath)
	MH_DEFINE_FUNC(ComputeShader::Create, Asset<ComputeShader>, const std::filesystem::path& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateAsset<HeadlessComputeShader>(filepath);
		case RendererAPI::API::OpenGL:
			return CreateAsset<OpenGLComputeShader>(filepath);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}