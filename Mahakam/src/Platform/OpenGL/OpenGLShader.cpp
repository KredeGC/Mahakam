#include "mhpch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Mahakam
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		MH_CORE_ASSERT(false, "Unknown shader type!");

		return 0;
	}

	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& sources)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(sources.size() <= 4, "Shader source too big");

		GLuint program = glCreateProgram();
		GLenum shaderIDs[4];

		int index = 0;
		for (auto& kv : sources)
		{
			std::string source = sortIncludes(kv.second);

			GLenum type = kv.first;
			GLuint shader = glCreateShader(type);

			const char* sourceC = source.c_str();
			glShaderSource(shader, 1, &sourceC, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				MH_CORE_ERROR("{0}", infoLog.data());
				MH_CORE_ASSERT(false, "Shader failed to compile!");

				break;
			}

			glAttachShader(program, shader);
			shaderIDs[index++] = shader;
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto& id : shaderIDs)
				glDeleteShader(id);

			MH_CORE_ERROR("{0}", infoLog.data());
			MH_CORE_ASSERT(false, "Shader failed to link!");

			return;
		}

		for (int i = 0; i < sources.size(); i++)
			glDetachShader(program, shaderIDs[i]);

		rendererID = program;
	}

	std::string OpenGLShader::sortIncludes(const std::string& source)
	{
		MH_PROFILE_FUNCTION();

		std::stringstream sourceStream;

		const char* typeToken = "#include \"";
		size_t typeTokenLength = strlen(typeToken);
		size_t lastPos = 0;
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			sourceStream << source.substr(lastPos, pos - lastPos);

			size_t end = source.find("\"", pos + typeTokenLength);
			size_t begin = pos + typeTokenLength;

			lastPos = end + 1;

			pos = source.find(typeToken, end);


			std::string includeFile = source.substr(begin, end - begin);

			std::string includeSource = readFile(includeFile);

			sourceStream << sortIncludes(includeSource);
		}

		sourceStream << source.substr(lastPos, source.size() - lastPos);

		return sourceStream.str();
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::parse(const std::string& source)
	{
		MH_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			MH_CORE_ASSERT(eol != std::string::npos, "Syntax error!");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			MH_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			sources[ShaderTypeFromString(type)] =
				source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return sources;
	}

	std::string OpenGLShader::readFile(const std::string& filepath)
	{
		MH_PROFILE_FUNCTION();

		std::string result;
		std::ifstream stream(filepath, std::ios::in | std::ios::binary);
		if (stream)
		{
			stream.seekg(0, std::ios::end);
			result.resize(stream.tellg());

			stream.seekg(0, std::ios::beg);
			stream.read(&result[0], result.size());

			stream.close();
		}
		else
		{
			MH_CORE_WARN("Could not open file {0}", filepath);
		}

		return result;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) : name(name)
	{
		MH_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;

		compile(sources);
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		MH_PROFILE_FUNCTION();

		std::string source = readFile(filepath);

		auto sources = parse(source);

		compile(sources);

		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastDot = filepath.rfind(".");
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

		name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::~OpenGLShader()
	{
		MH_PROFILE_FUNCTION();

		glDeleteProgram(rendererID);
	}

	void OpenGLShader::bind() const
	{
		MH_PROFILE_FUNCTION();

		glUseProgram(rendererID);
	}

	void OpenGLShader::unbind() const
	{
		MH_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::bindBuffer(const std::string& name, int slot)
	{
		uint32_t blockID = glGetUniformBlockIndex(rendererID, name.c_str());
		glUniformBlockBinding(rendererID, blockID, slot);
	}

	void OpenGLShader::setViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
		setUniformMat4("u_m4_V", viewMatrix);
		setUniformMat4("u_m4_P", projectionMatrix);
	}

	void OpenGLShader::setUniformMat3(const std::string& name, const glm::mat3& value)
	{
		GLint id = glGetUniformLocation(rendererID, name.c_str());
		if (id != -1)
			glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::setUniformMat4(const std::string& name, const glm::mat4& value)
	{
		GLint id = glGetUniformLocation(rendererID, name.c_str());
		if (id != -1)
			glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::setUniformInt(const std::string& name, int value)
	{
		GLint id = glGetUniformLocation(rendererID, name.c_str());
		if (id != -1)
			glUniform1i(id, value);
	}

	void OpenGLShader::setUniformFloat(const std::string& name, float value)
	{
		GLint id = glGetUniformLocation(rendererID, name.c_str());
		if (id != -1)
			glUniform1f(id, value);
	}

	void OpenGLShader::setUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint id = glGetUniformLocation(rendererID, name.c_str());
		if (id != -1)
			glUniform2f(id, value.x, value.y);
	}

	void OpenGLShader::setUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint id = glGetUniformLocation(rendererID, name.c_str());
		if (id != -1)
			glUniform3f(id, value.x, value.y, value.z);
	}

	void OpenGLShader::setUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint id = glGetUniformLocation(rendererID, name.c_str());
		if (id != -1)
			glUniform4f(id, value.x, value.y, value.z, value.w);
	}
}