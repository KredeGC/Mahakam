#include "Mahakam/mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLShader.h"
#include "OpenGLShaderDataTypes.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/ShaderUtility.h"
#include "Mahakam/Renderer/Texture.h"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

#define ENABLE_HLSL
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

#include <filesystem>
#include <fstream>

namespace Mahakam
{
	OpenGLShader::OpenGLShader(const std::filesystem::path& filepath)
		: m_Filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

		// Naming
		m_Name = m_Filepath.stem().string();

		// Read YAML file for shader passes
		UnorderedMap<std::string, SourceDefinition> sources;
		if (ParseYAMLFile(m_Filepath, sources, m_Properties))
		{
			// Compile binaries for each shader pass
			for (auto& [shaderPass, sourceDef]: sources)
				m_ShaderPasses[shaderPass] = CompileBinary(sourceDef.Sources, sourceDef.Defines);
		}
	}

	OpenGLShader::OpenGLShader(OpenGLShader&& other) noexcept :
		m_RendererID(other.m_RendererID),
		m_Filepath(std::move(other.m_Filepath)),
		m_Name(std::move(other.m_Name)),
		m_ShaderPasses(std::move(other.m_ShaderPasses)),
		m_UniformIDCache(std::move(other.m_UniformIDCache)),
		m_Properties(std::move(other.m_Properties)),
		m_UniformSize(other.m_UniformSize)
	{
		other.m_UniformSize = 0;
	}

	OpenGLShader::~OpenGLShader()
	{
		MH_PROFILE_FUNCTION();

		for (auto& pass : m_ShaderPasses)
		{
			MH_GL_CALL(glDeleteProgram(pass.second));
		}
	}

	OpenGLShader& OpenGLShader::operator=(OpenGLShader&& rhs) noexcept
	{
		m_RendererID = rhs.m_RendererID;
		m_Filepath = std::move(rhs.m_Filepath);
		m_Name = std::move(rhs.m_Name);
		m_ShaderPasses = std::move(rhs.m_ShaderPasses);
		m_UniformIDCache = std::move(rhs.m_UniformIDCache);
		m_Properties = std::move(rhs.m_Properties);
		m_UniformSize = rhs.m_UniformSize;

		rhs.m_UniformSize = 0;

		return *this;
	}

	void OpenGLShader::Bind(const std::string& shaderPass)
	{
		auto passIter = m_ShaderPasses.find(shaderPass);
		if (passIter != m_ShaderPasses.end())
		{
			m_RendererID = passIter->second;
			MH_GL_CALL(glUseProgram(m_RendererID));
		}
		else
		{
			MH_WARN("Attempted to use unknown shader pass: {0} on {1}", shaderPass, m_Name);
		}
	}

	bool OpenGLShader::HasShaderPass(const std::string& shaderPass) const
	{
		auto iter = m_ShaderPasses.find(shaderPass);
		if (iter != m_ShaderPasses.end())
			return true;

		return false;
	}

	void OpenGLShader::SetTexture(const std::string& name, Texture& tex)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glBindTextureUnit(slot, tex.GetRendererID()));
	}

	void OpenGLShader::SetUniformMat3(const std::string& name, const glm::mat3& value)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glUniformMatrix3fv(slot, 1, GL_FALSE, glm::value_ptr(value)));
	}

	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& value)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glUniformMatrix4fv(slot, 1, GL_FALSE, glm::value_ptr(value)));
	}

	void OpenGLShader::SetUniformInt(const std::string& name, int value)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glUniform1i(slot, value));
	}

	void OpenGLShader::SetUniformFloat(const std::string& name, float value)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glUniform1f(slot, value));
	}

	void OpenGLShader::SetUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glUniform2f(slot, value.x, value.y));
	}

	void OpenGLShader::SetUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glUniform3f(slot, value.x, value.y, value.z));
	}

	void OpenGLShader::SetUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glUniform4f(slot, value.x, value.y, value.z, value.w));
	}

	uint32_t OpenGLShader::CompileBinary(const UnorderedMap<ShaderStage, std::string>& sources, const std::string& directives)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		if (sources.empty())
		{
			MH_WARN("At least one shader source is required to compile!");
			return 0;
		}

		UnorderedMap<ShaderStage, std::vector<uint32_t>> spirv;

		CompileSPIRV(spirv, { sources, directives });

		GLuint program = glCreateProgram();

		// Create shader stages
		TrivialVector<GLuint> shaderIDs;
		shaderIDs.reserve(sources.size());
		for (auto& kv : spirv)
		{
			GLenum stage = ShaderStageToOpenGLStage(kv.first);
			GLuint shader = glCreateShader(stage);

			// AMD GPUs do not load SPIR-V shaders correctly in OpenGL
			// https://github.com/TheCherno/Hazel/issues/440
			// TODO: Make this an automatic check based on drivers?
#if true // ifdef MH_AMD_GPU
			spirv_cross::CompilerGLSL glsl(kv.second);

			spirv_cross::CompilerGLSL::Options options;
			options.version = 430;
			glsl.set_common_options(options);

			std::string source = glsl.compile();

			const char* sourceA[]{ source.c_str() };

			MH_GL_CALL(glShaderSource(shader, 1, sourceA, 0));

			MH_GL_CALL(glCompileShader(shader));
#else
			MH_GL_CALL(glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, kv.second.data(), (int)(kv.second.size() * sizeof(uint32_t))));

			MH_GL_CALL(glSpecializeShaderARB(shader, "main", 0, nullptr, nullptr));
#endif

			GLint isCompiled = 0;
			MH_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				MH_GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));

				TrivialVector<GLchar> infoLog(maxLength);
				MH_GL_CALL(glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]));

				MH_GL_CALL(glDeleteShader(shader));

				spirv_cross::CompilerGLSL glsl(spirv[ShaderStage::Vertex]);

				spirv_cross::CompilerGLSL::Options options;
				options.version = 430;
				glsl.set_common_options(options);

				std::string source = glsl.compile();

				MH_WARN("{0}\r\n\r\n{1}\r\n\r\n{2}", source, directives, infoLog.data());

				break;
			}

			MH_GL_CALL(glAttachShader(program, shader));
			shaderIDs.push_back(shader);
		}

		MH_GL_CALL(glLinkProgram(program));

		// Link shader program
		GLint isLinked = 0;
		MH_GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked));
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			MH_GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

			TrivialVector<GLchar> infoLog(maxLength);
			MH_GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));

			MH_GL_CALL(glDeleteProgram(program));

			for (auto& id : shaderIDs)
				MH_GL_CALL(glDeleteShader(id));

			MH_WARN("{0}\r\n\r\n{1}", directives, infoLog.data());

			return 0;
		}

		for (int i = 0; i < sources.size(); i++)
			MH_GL_CALL(glDetachShader(program, shaderIDs[i]));

		// Reflect the fragment shader using SPIR V
		m_UniformSize = ReflectSPIRV(spirv[ShaderStage::Fragment], m_Properties);

		return program;
	}

	int OpenGLShader::GetUniformLocation(const std::string& name)
	{
		auto cacheIter = m_UniformIDCache.find(name);
		if (cacheIter != m_UniformIDCache.end() && cacheIter->second != -1)
			return cacheIter->second;

		// Get the ID of a shader pass (prefer our current shader pass)
		int uniformID = -1;
		if (m_RendererID != ~0)
			MH_GL_CALL(uniformID = glGetUniformLocation(m_RendererID, name.c_str()));

		// Otherwise try to find it in other shader passes
		if (uniformID == -1)
		{
			for (auto& [pass, id] : m_ShaderPasses)
			{
				if (id == m_RendererID || id == ~0) continue;
				MH_GL_CALL(uniformID = glGetUniformLocation(id, name.c_str()));
				if (uniformID != -1) break;
			}
		}

		m_UniformIDCache[name] = uniformID;

		// Inform the user that a property is unused
		if (cacheIter == m_UniformIDCache.end() && uniformID == -1)
			MH_WARN("Shader {0} Uniform {1} unused or optimized away", m_Filepath.string(), name);

		return uniformID;
	}
}