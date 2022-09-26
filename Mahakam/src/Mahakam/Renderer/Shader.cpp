#include "Mahakam/mhpch.h"
#include "Shader.h"

#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/Headless/HeadlessShader.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Mahakam
{
	//Ref<Shader> Shader::Create(const std::string& filepath, const std::initializer_list<std::string>& keywords)
	MH_DEFINE_FUNC(Shader::CreateFilepath, Ref<Shader>, const std::filesystem::path& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateRef<HeadlessShader>(filepath);
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(filepath);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	uint32_t ShaderDataTypeComponentCount(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 1;
		case ShaderDataType::Float2:
			return 2;
		case ShaderDataType::Float3:
			return 3;
		case ShaderDataType::Float4:
			return 4;
		case ShaderDataType::Mat3:
			return 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4;
		case ShaderDataType::Int:
			return 1;
		case ShaderDataType::Int2:
			return 2;
		case ShaderDataType::Int3:
			return 3;
		case ShaderDataType::Int4:
			return 4;
		case ShaderDataType::Bool:
			return 1;
		default:
			MH_CORE_BREAK("Unknown shader data type!");
			return 0;
		}
	}

	uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 8;
		case ShaderDataType::Float3:
			return 12;
		case ShaderDataType::Float4:
			return 16;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
		case ShaderDataType::Int:
			return 4;
		case ShaderDataType::Int2:
			return 8;
		case ShaderDataType::Int3:
			return 12;
		case ShaderDataType::Int4:
			return 16;
		case ShaderDataType::Bool:
			return 1;
		default:
			MH_CORE_BREAK("Unknown shader data type!");
			return 0;
		}
	}
}