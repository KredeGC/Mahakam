#include "mhpch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Mahakam
{
	Ref<Shader> Shader::Create(const std::string& filepath, const std::initializer_list<std::string>& defines)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(filepath, defines);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}
}