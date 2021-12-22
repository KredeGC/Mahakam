#include "mhpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Mahakam
{
	Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSrouce)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertexSource, fragmentSrouce);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

		return nullptr;
	}

	Ref<Shader> Shader::create(const std::string& filepath)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_ASSERT(false, "Renderer API not supported!");
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(filepath);
		}

		MH_CORE_ASSERT(false, "Unknown renderer API!");

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