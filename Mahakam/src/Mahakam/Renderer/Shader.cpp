#include "Mahakam/mhpch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Mahakam
{
	//Ref<Shader> Shader::Create(const std::string& filepath, const std::initializer_list<std::string>& keywords)
	MH_DEFINE_FUNC(Shader::CreateFilepath, Ref<Shader>, const std::filesystem::path& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(filepath);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};
}