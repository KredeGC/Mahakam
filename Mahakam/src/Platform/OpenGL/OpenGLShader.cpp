#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLShader.h"
#include "OpenGLUtility.h"
#include "OpenGLShaderDataTypes.h"
#include "Mahakam/Core/Utility.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/Texture.h"

#include <filesystem>
#include <fstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <yaml-cpp/yaml.h>

namespace Mahakam
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		MH_CORE_BREAK("Unknown shader type!");

		return 0;
	}

	static std::string GLShaderStageToFileExtension(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:
			return ".vert";
		case GL_FRAGMENT_SHADER:
			return ".frag";
		}

		MH_CORE_BREAK("Shader stage not supported!");
		return "";
	}

	OpenGLShader::OpenGLShader(const std::filesystem::path& filepath, const std::initializer_list<std::string>& keywords)
		: m_Filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

		// Naming
		m_Name = filepath.stem().string();

		std::vector<std::string> features = keywords;
		features.emplace_back("");

		// Read YAML file for shader passes
		ParseYAMLFile(filepath, features);
	}

	OpenGLShader::~OpenGLShader()
	{
		MH_PROFILE_FUNCTION();

		for (auto& pass : m_ShaderPasses)
		{
			for (auto& shader : pass.second)
			{
				MH_GL_CALL(glDeleteProgram(shader.second));
			}
		}
	}

	void OpenGLShader::Bind(const std::string& shaderPass, const std::string& variant)
	{
		auto passIter = m_ShaderPasses.find(shaderPass);
		if (passIter != m_ShaderPasses.end())
		{
			auto& variants = passIter->second;

			auto variantIter = variants.find(variant);
			if (variantIter != variants.end())
			{
				m_RendererID = variantIter->second;
				MH_GL_CALL(glUseProgram(m_RendererID));
			}
			else
			{
				MH_CORE_BREAK("Attempted to use uncompiled shader variant!");
			}
		}
		else
		{
			MH_CORE_BREAK("Attempted to use unknown shader pass!");
		}
	}

	bool OpenGLShader::HasShaderPass(const std::string& shaderPass) const
	{
		auto iter = m_ShaderPasses.find(shaderPass);
		if (iter != m_ShaderPasses.end())
			return true;

		return false;
	}

	void OpenGLShader::SetTexture(const std::string& name, Asset<Texture> tex)
	{
		int slot = GetUniformLocation(name);
		if (slot != -1)
			MH_GL_CALL(glBindTextureUnit(slot, tex->GetRendererID()));
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

	uint32_t OpenGLShader::CompileBinary(const std::filesystem::path& cachePath, const UnorderedMap<uint32_t, std::string>& sources, const std::string& directives)
	{
		MH_PROFILE_RENDERING_FUNCTION();

		MH_CORE_ASSERT(sources.size() > 0, "At least one shader source is required to compile!");

		GLuint program = glCreateProgram();

#if MH_DEBUG // Always recompile in debug mode
		if (true)
#else
		if (!FileUtility::Exists(cachePath))
#endif
		{
			// Create shader stages
			std::vector<GLuint> shaderIDs;
			shaderIDs.reserve(sources.size());
			for (auto& kv : sources)
			{
				std::string original = OpenGLUtility::SortIncludes(kv.second);
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
					MH_CORE_BREAK("Shader failed to compile!");

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

				std::vector<GLchar> infoLog(maxLength);
				MH_GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]));

				MH_GL_CALL(glDeleteProgram(program));

				for (auto& id : shaderIDs)
					MH_GL_CALL(glDeleteShader(id));

				MH_CORE_ERROR("{0}\r\n\r\n{1}", directives, infoLog.data());
				MH_CORE_BREAK("Shader failed to link!");

				return 0;
			}

			for (int i = 0; i < sources.size(); i++)
				MH_GL_CALL(glDetachShader(program, shaderIDs[i]));

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

				MH_CORE_ERROR("{0}\r\n\r\n{1}", directives, infoLog.data());
				MH_CORE_BREAK("Shader failed to link!");

				return 0;
			}
		}


		// Shader reflection
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

			auto iter = m_Properties.find(name);
			if (iter != m_Properties.end())
				iter->second.DataType = OpenGLDataTypeToShaderDataType(values[1]);
		}

		return program;
	}

	std::string OpenGLShader::ParseDefaultValue(const YAML::Node& node)
	{
		if (node)
		{
			if (node.IsScalar())
			{
				return node.Scalar();
			}
			else if (node.IsSequence())
			{
				std::stringstream value;
				for (auto iter = node.begin(); iter != node.end();)
				{
					value << ParseDefaultValue(*iter);
					if (++iter != node.end())
						value << ",";
				}
				return "[" + value.str() + "]";
			}
		}

		return "";
	}

	void OpenGLShader::ParseYAMLFile(const std::filesystem::path& filepath, const std::vector<std::string>& keywords)
	{
		YAML::Node rootNode;
		try
		{
			rootNode = YAML::LoadFile(filepath.string());
		}
		catch (YAML::Exception e)
		{
			MH_CORE_WARN("OpenGLShader encountered exception trying to parse YAML file {0}: {1}", filepath, e.msg);
		}

		MH_CORE_ASSERT(rootNode && rootNode.size() > 0, "Loaded empty shader file! Path may be wrong!");

		UnorderedMap<std::string, std::string> keywordPermutations = ParseShaderKeywords(keywords);

		// Read properties
		auto propertiesNode = rootNode["Properties"];

		MH_CORE_INFO("Loading properties for shader: {0}", m_Name);
		for (auto propertyNode : propertiesNode)
		{
			std::string propertyName = propertyNode.first.as<std::string>();

			YAML::Node typeNode = propertyNode.second["Type"];
			YAML::Node minNode = propertyNode.second["Min"];
			YAML::Node maxNode = propertyNode.second["Max"];
			YAML::Node defaultNode = propertyNode.second["Default"];

			ShaderPropertyType propertyType = ShaderPropertyType::Default;
			if (typeNode)
			{
				std::string typeString = typeNode.as<std::string>();
				if (typeString == "Color")			propertyType = ShaderPropertyType::Color;
				else if (typeString == "HDR")		propertyType = ShaderPropertyType::HDR;
				else if (typeString == "Vector")	propertyType = ShaderPropertyType::Vector;
				else if (typeString == "Range")		propertyType = ShaderPropertyType::Range;
				else if (typeString == "Drag")		propertyType = ShaderPropertyType::Drag;
				else if (typeString == "Texture")	propertyType = ShaderPropertyType::Texture;
				else if (typeString == "Normal")	propertyType = ShaderPropertyType::Normal;
				else if (typeString == "Default")	propertyType = ShaderPropertyType::Default;
			}

			float min = 0;
			if (minNode)
				min = minNode.as<float>();

			float max = 0;
			if (maxNode)
				max = maxNode.as<float>();

			std::string defaultValue = ParseDefaultValue(defaultNode);

			m_Properties[propertyName] = { propertyType, ShaderDataType::None, min, max, "Value: " + defaultValue };

			MH_CORE_INFO("  {0}: {1}", propertyName, defaultValue);
		}

		// Read shader passes
		auto passesNode = rootNode["Passes"];

		for (auto shaderPassNode : passesNode)
		{
			std::string shaderPassName = shaderPassNode.first.as<std::string>();

			// Read shaderpass defines
			std::stringstream shaderPassDefines;
			auto definesNode = shaderPassNode.second["Defines"];
			if (definesNode)
			{
				for (auto define : definesNode)
					shaderPassDefines << "#define " + define.as<std::string>() + "\n";
			}

			const std::string partialPath = filepath.string() + "_" + shaderPassName;

			// Read and compile include files
			auto includesNode = shaderPassNode.second["Includes"];
			if (includesNode)
			{
				// Read and parse source files
				std::stringstream source;
				for (auto includeNode : includesNode)
				{
					std::string shaderPath = includeNode.as<std::string>();

					source << OpenGLUtility::ReadFile(shaderPath);
				}

				auto sources = ParseGLSLFile(source.str());

				// Generate shaders for each shader pass * each keyword combination
				for (const auto& directives : keywordPermutations)
					m_ShaderPasses[shaderPassName][directives.first] = CompileBinary(FileUtility::GetCachePath(partialPath + directives.first), sources, shaderPassDefines.str() + directives.second);
			}
		}
	}

	UnorderedMap<std::string, std::string> OpenGLShader::ParseShaderKeywords(const std::vector<std::string>& keywords)
	{
		const uint64_t length = keywords.size();
		const uint64_t bits = 1ULL << length;

		UnorderedMap<std::string, std::string> result;
		for (uint64_t i = 1; i < bits; ++i)
		{
			std::stringstream combinedTag;
			std::stringstream combinedDefines;
			for (uint64_t bit = 0; bit < length; ++bit)
			{
				if (i & (1ULL << bit) && !keywords[bit].empty())
				{
					combinedTag << keywords[bit];
					combinedDefines << "#define " << keywords[bit] << "\n";
				}
			}
			result[combinedTag.str()] = combinedDefines.str();
		}

		return result;
	}

	UnorderedMap<uint32_t, std::string> OpenGLShader::ParseGLSLFile(const std::string& source)
	{
		MH_PROFILE_FUNCTION();

		UnorderedMap<uint32_t, std::string> sources;

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

	int OpenGLShader::GetUniformLocation(const std::string& name)
	{
		auto iter = m_UniformIDCache.find(name);
		if (iter != m_UniformIDCache.end() && iter->second != -1)
			return iter->second;

		int uniformID = glGetUniformLocation(m_RendererID, name.c_str());

		m_UniformIDCache[name] = uniformID;

		// Inform the user that a property is unused
		if (iter == m_UniformIDCache.end() && uniformID == -1)
			MH_CORE_WARN("Shader {0} Uniform {1} unused or optimized away", m_Filepath.string(), name);

		return uniformID;
	}
}