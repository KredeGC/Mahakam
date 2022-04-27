#include "mhpch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Mahakam
{
	//Ref<Shader> Shader::Create(const std::string& filepath, const std::initializer_list<std::string>& keywords)
	MH_DEFINE_FUNC(Shader::CreateFilepath, Ref<Shader>, const std::string& filepath, const std::initializer_list<std::string>& keywords)
	{
		MH_OVERRIDE_FUNC(ShaderCreate, filepath, keywords);

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return static_cast<Ref<Shader>>(CreateRef<OpenGLShader>(filepath, keywords));
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return Ref<Shader>(nullptr);
	};
}