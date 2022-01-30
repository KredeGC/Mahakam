#include "mhpch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Mahakam
{
	Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSrouce)
	{
		switch (RendererAPI::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(name, vertexSource, fragmentSrouce);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	}

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



	void ShaderLibrary::add(const Ref<Shader>& shader)
	{
		const std::string& name = shader->getName();

		MH_CORE_ASSERT(shaders.find(name) == shaders.end(), "Shader already exists!");

		shaders[name] = shader;
	}
	
	Ref<Shader> ShaderLibrary::load(const std::string& filepath)
	{
		auto shader = Shader::create(filepath);

		add(shader);

		return shader;
	}
	
	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::create(filepath);

		MH_CORE_ASSERT(shaders.find(name) == shaders.end(), "Shader already exists!");

		shaders[name] = shader;

		return shader;
	}
	
	Ref<Shader> ShaderLibrary::get(const std::string& name)
	{
		MH_CORE_ASSERT(shaders.find(name) != shaders.end(), "Shader doesn't exist!");
		return shaders[name];
	}
}