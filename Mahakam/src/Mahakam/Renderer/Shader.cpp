#include "mhpch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Mahakam
{
	Ref<Shader> Shader::create(const std::string& filepath, const std::initializer_list<std::string>& defines)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(filepath, defines);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}


#pragma region ShaderLibrary
	std::unordered_map<std::string, Ref<Shader>> ShaderLibrary::shaders;

	Ref<Shader> ShaderLibrary::load(const std::string& filepath, const std::initializer_list<std::string>& defines)
	{
		auto& iter = shaders.find(filepath);
		if (iter == shaders.end())
		{
			Ref<Shader> shader = Shader::create(filepath);

			shaders[filepath] = shader;

			return shader;
		}
	}
	
	Ref<Shader> ShaderLibrary::get(const std::string& filepath)
	{
		MH_CORE_ASSERT(shaders.find(filepath) != shaders.end(), "Shader doesn't exist!");
		return shaders[filepath];
	}
#pragma endregion
}