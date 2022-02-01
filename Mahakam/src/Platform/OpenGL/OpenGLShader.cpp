#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLShader.h"

#include "OpenGLShaderDataTypes.h"

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

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource) : name(name)
	{
		MH_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSource;
		sources[GL_FRAGMENT_SHADER] = fragmentSource;

		uint32_t program = compile(sources, "");

		shaderVariants[""] = program;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath, const std::initializer_list<std::string>& variants)
	{
		MH_PROFILE_FUNCTION();

		// Naming
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastDot = filepath.rfind(".");
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

		name = filepath.substr(lastSlash, count);

		// Multiple shader variants
		for (const std::string& combinedDefines : variants)
		{
			std::stringstream definitions;

			// Multiple definitions
			if (combinedDefines.size() > 0)
			{
				std::string delim = ";";
				size_t start = 0;
				size_t end = combinedDefines.find(delim);
				while (end != std::string::npos)
				{
					definitions << "#define " << combinedDefines.substr(start, end - start) << std::endl;
					start = end + delim.length();
					end = combinedDefines.find(delim, start);
				}

				definitions << "#define " << combinedDefines.substr(start, end) << std::endl;
			}

			std::string source = readFile(filepath);

			auto sources = parse(source);

			uint32_t program = compile(sources, definitions.str());

			shaderVariants[combinedDefines] = program;
		}
	}

	OpenGLShader::~OpenGLShader()
	{
		MH_PROFILE_FUNCTION();

		for (auto& pair : shaderVariants)
			MH_GL_CALL(glDeleteProgram(pair.second));
	}

	void OpenGLShader::bind(const std::string& variant)
	{
		uint32_t program = shaderVariants.at(variant);
		rendererID = program;

		MH_GL_CALL(glUseProgram(program));
	}

	void OpenGLShader::setViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
		setUniformMat4("u_m4_V", viewMatrix);
		setUniformMat4("u_m4_P", projectionMatrix);
	}

	void OpenGLShader::setTexture(const std::string& name, Ref<Texture> tex)
	{
		int slot = getUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glBindTextureUnit(slot, tex->getRendererID()));
	}

	void OpenGLShader::setUniformMat3(const std::string& name, const glm::mat3& value)
	{
		MH_GL_CALL(glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)));
	}

	void OpenGLShader::setUniformMat4(const std::string& name, const glm::mat4& value)
	{
		MH_GL_CALL(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value)));
	}

	void OpenGLShader::setUniformInt(const std::string& name, int value)
	{
		MH_GL_CALL(glUniform1i(getUniformLocation(name), value));
	}

	void OpenGLShader::setUniformFloat(const std::string& name, float value)
	{
		MH_GL_CALL(glUniform1f(getUniformLocation(name), value));
	}

	void OpenGLShader::setUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		MH_GL_CALL(glUniform2f(getUniformLocation(name), value.x, value.y));
	}

	void OpenGLShader::setUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		MH_GL_CALL(glUniform3f(getUniformLocation(name), value.x, value.y, value.z));
	}

	void OpenGLShader::setUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		MH_GL_CALL(glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w));
	}

	uint32_t OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& sources, const std::string& directives)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(sources.size() <= 4, "Shader source too big!");

		GLuint program = glCreateProgram();
		GLenum shaderIDs[4];

		int index = 0;
		for (auto& kv : sources)
		{
			std::string original = sortIncludes(kv.second);
			size_t firstNewline = original.find("\n") + 1;
			std::string version = original.substr(0, firstNewline);
			std::string body = original.substr(firstNewline, original.size());
			std::string source = version + directives + body;

			GLenum type = kv.first;
			GLuint shader = glCreateShader(type);

			const char* sourceC = source.c_str();
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

				MH_CORE_ERROR("{0}\r\n\r\n{1}\r\n\r\n{2}", source, directives, infoLog.data());
				MH_CORE_ASSERT(false, "Shader failed to compile!");

				break;
			}

			MH_GL_CALL(glAttachShader(program, shader));
			shaderIDs[index++] = shader;
		}

		MH_GL_CALL(glLinkProgram(program));

		GLint isLinked = 0;
		MH_GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked));
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			MH_GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

			std::vector<GLchar> infoLog(maxLength);
			MH_GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));

			MH_GL_CALL(glDeleteProgram(program));

			for (auto& id : shaderIDs)
				MH_GL_CALL(glDeleteShader(id));

			MH_CORE_ERROR("{0}\r\n\r\n{1}", directives, infoLog.data());
			MH_CORE_ASSERT(false, "Shader failed to link!");

			return 0;
		}

		for (int i = 0; i < sources.size(); i++)
			MH_GL_CALL(glDetachShader(program, shaderIDs[i]));


		if (properties.elements.empty())
		{
			MH_CORE_INFO("Loading properties for shader: {0}", name);

			GLint numUniforms = 0;
			MH_GL_CALL(glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms));
			const GLenum props[4] = { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };

			for (int unif = 0; unif < numUniforms; ++unif)
			{
				GLint values[4];
				MH_GL_CALL(glGetProgramResourceiv(program, GL_UNIFORM, unif, 4, props, 4, NULL, values));

				// Skip any uniforms that are in a block.
				if (values[0] != -1)
					continue;

				// Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
				// C++11 would let you use a std::string directly.
				std::vector<char> nameData(values[2]);
				MH_GL_CALL(glGetProgramResourceName(program, GL_UNIFORM, unif, (GLsizei)nameData.size(), NULL, &nameData[0]));
				std::string name(nameData.begin(), nameData.end() - 1);

				ShaderDataType dataType = OpenGLDataTypeToShaderDataType(values[1]);

				MH_CORE_INFO("  layout(location = {0}) {1}", (uint32_t)values[3], name);

				properties.elements.push_back({ OpenGLDataTypeToShaderDataType(values[1]), name, (uint32_t)values[3] });
			}
		}

		return program;
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

	int OpenGLShader::getUniformLocation(const std::string& name)
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