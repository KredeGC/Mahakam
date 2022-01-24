#pragma once
#include "Mahakam/Core/Log.h"
#include "Mahakam/Renderer/ShaderDataTypes.h"

#include <glad/glad.h>

// ONLY INCLUDE IN CPP FILES

namespace Mahakam
{
	static GLenum ShaderDataTypeToOpenGLDataType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return GL_FLOAT;
		case ShaderDataType::Float2:
			return GL_FLOAT_VEC2;
		case ShaderDataType::Float3:
			return GL_FLOAT_VEC4;
		case ShaderDataType::Float4:
			return GL_FLOAT_VEC4;
		case ShaderDataType::Mat3:
			return GL_FLOAT_MAT3;
		case ShaderDataType::Mat4:
			return GL_FLOAT_MAT4;
		case ShaderDataType::Int:
			return GL_INT;
		case ShaderDataType::Int2:
			return GL_INT_VEC2;
		case ShaderDataType::Int3:
			return GL_INT_VEC3;
		case ShaderDataType::Int4:
			return GL_INT_VEC4;
		case ShaderDataType::Bool:
			return GL_BOOL;
		case ShaderDataType::Sampler2D:
			return GL_SAMPLER_2D;
		case ShaderDataType::SamplerCube:
			return GL_SAMPLER_CUBE;
		}

		MH_CORE_BREAK("Unknown ShaderDataType provided!");
		return 0;
	}

	static ShaderDataType OpenGLDataTypeToShaderDataType(GLenum type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return ShaderDataType::Float;
		case GL_FLOAT_VEC2:
			return ShaderDataType::Float2;
		case GL_FLOAT_VEC3:
			return ShaderDataType::Float3;
		case GL_FLOAT_VEC4:
			return ShaderDataType::Float4;
		case GL_FLOAT_MAT3:
			return ShaderDataType::Mat3;
		case GL_FLOAT_MAT4:
			return ShaderDataType::Mat4;
		case GL_INT:
			return ShaderDataType::Int;
		case GL_INT_VEC2:
			return ShaderDataType::Int2;
		case GL_INT_VEC3:
			return ShaderDataType::Int3;
		case GL_INT_VEC4:
			return ShaderDataType::Int4;
		case GL_BOOL:
			return ShaderDataType::Bool;
		case GL_SAMPLER_2D:
			return ShaderDataType::Sampler2D;
		case GL_SAMPLER_CUBE:
			return ShaderDataType::SamplerCube;
		}

		MH_CORE_BREAK("Unknown OpenGL data type provided!");
		return ShaderDataType::None;
	}
}