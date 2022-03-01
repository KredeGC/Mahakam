#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLComputeShader.h"
#include "OpenGLUtility.h"

#include <filesystem>
#include <fstream>

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLComputeShader::OpenGLComputeShader(const std::string& filepath)
		: filepath(filepath)
	{
		// Naming
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastDot = filepath.rfind(".");
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

		name = filepath.substr(lastSlash, count);

		const std::string cachePath = "cache/compute/" + name + ".dat";

		std::string src = OpenGLUtility::ReadFile(filepath);

		src = OpenGLUtility::SortIncludes(src);

		CompileBinary(cachePath, src);
	}

	OpenGLComputeShader::~OpenGLComputeShader()
	{
		MH_GL_CALL(glDeleteProgram(rendererID));
	}

	void OpenGLComputeShader::Bind() const
	{
		MH_GL_CALL(glUseProgram(rendererID));
	}

	void OpenGLComputeShader::Dispatch(uint32_t x, uint32_t y, uint32_t z)
	{
		MH_GL_CALL(glDispatchCompute(x, y, z));
		MH_GL_CALL(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	}

	void OpenGLComputeShader::SetTexture(const std::string& name, Ref<Texture> tex)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glBindTextureUnit(slot, tex->GetRendererID()));
	}

	void OpenGLComputeShader::CompileBinary(const std::string& cachePath, const std::string& src)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		GLuint program = glCreateProgram();

		if (!std::filesystem::exists(cachePath))
		{
			// Create shader stages
			GLuint shader = glCreateShader(GL_COMPUTE_SHADER);

			const char* sourceC = src.c_str();
			MH_GL_CALL(glShaderSource(shader, 1, &sourceC, 0));

			MH_GL_CALL(glCompileShader(shader));

			GLint isCompiled = 0;
			MH_GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				MH_GL_CALL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength));

				std::vector<GLchar> infoLog(maxLength);
				MH_GL_CALL(glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]));

				MH_GL_CALL(glDeleteShader(shader));

				MH_CORE_ERROR("{0}\r\n\r\n{1}", src, infoLog.data());
				MH_CORE_BREAK("Shader failed to compile!");

				return;
			}

			MH_GL_CALL(glAttachShader(program, shader));

			MH_GL_CALL(glLinkProgram(program));

			// Link shader program
			GLint isLinked = 0;
			MH_GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked));
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				MH_GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

				std::vector<GLchar> infoLog(maxLength);
				MH_GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));

				MH_GL_CALL(glDeleteProgram(program));

				MH_GL_CALL(glDeleteShader(shader));

				MH_CORE_ERROR("{0}\r\n\r\n{1}", src, infoLog.data());
				MH_CORE_BREAK("Shader failed to link!");

				return;
			}

			MH_GL_CALL(glDetachShader(program, shader));

			rendererID = program;

			// Write to cache
			int bufSize;
			glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &bufSize);

			char* data = new char[bufSize];
			uint32_t format;

			glGetProgramBinary(program, bufSize, nullptr, &format, data);

			std::ofstream out(cachePath, std::ios::out | std::ios::binary);
			out.write((char*)&format, sizeof(uint32_t));
			out.write(data, bufSize);
		}
		else
		{
			std::ifstream in(cachePath, std::ios::binary);
			in.seekg(0, std::ios::end);
			uint32_t bufSize = (uint32_t)in.tellg() - sizeof(uint32_t);
			in.seekg(0, std::ios::beg);

			char* data = new char[bufSize];
			uint32_t format;

			in.read((char*)&format, sizeof(uint32_t));
			in.read(data, bufSize);

			glProgramBinary(program, format, data, bufSize);

			GLint isLinked;
			MH_GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked));
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				MH_GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

				std::vector<GLchar> infoLog(maxLength);
				MH_GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));

				MH_GL_CALL(glDeleteProgram(program));

				MH_CORE_ERROR("{0}\r\n\r\n{1}", src, infoLog.data());
				MH_CORE_BREAK("Shader failed to link!");

				return;
			}

			rendererID = program;
		}
	}

	int OpenGLComputeShader::GetUniformLocation(const std::string& name)
	{
		if (uniformIDCache.find(name) != uniformIDCache.end())
			return uniformIDCache[name];

		int uniformID = glGetUniformLocation(rendererID, name.c_str());

		if (uniformID != -1)
			uniformIDCache[name] = uniformID;
		else
			MH_CORE_WARN("Uniform {0} unused or optimized away", name);

		return uniformID;
	}
}