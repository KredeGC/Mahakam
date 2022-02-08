#include "mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLShader.h"

#include "OpenGLShaderDataTypes.h"

#include <filesystem>
#include <fstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include <yaml-cpp/yaml.h>

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

	static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:
			return shaderc_glsl_vertex_shader;
		case GL_FRAGMENT_SHADER:
			return shaderc_glsl_fragment_shader;
		}

		MH_CORE_BREAK("Shader stage not supported!");
		return (shaderc_shader_kind)0;
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

	OpenGLShader::OpenGLShader(const std::string& filepath, const std::initializer_list<std::string>& keywords)
		: filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

		// Naming
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastDot = filepath.rfind(".");
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;

		name = filepath.substr(lastSlash, count);

		std::vector<std::string> features = keywords;
		features.emplace_back("");

		// Read YAML file for shader passes
		ParseYAMLFile(filepath, features);
	}

	OpenGLShader::~OpenGLShader()
	{
		MH_PROFILE_FUNCTION();

		for (auto& pass : shaderPasses)
		{
			for (auto& shader : pass.second)
			{
				MH_GL_CALL(glDeleteProgram(shader.second));
			}
		}
	}

	void OpenGLShader::Bind(const std::string& shaderPass, const std::string& variant)
	{
		auto& passIter = shaderPasses.find(shaderPass);
		if (passIter != shaderPasses.end())
		{
			auto& variants = passIter->second;

			auto& variantIter = variants.find(variant);
			if (variantIter != variants.end())
			{
				rendererID = variantIter->second;
				MH_GL_CALL(glUseProgram(rendererID));
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

	void OpenGLShader::SetViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
	{
		SetUniformMat4("u_m4_V", viewMatrix);
		SetUniformMat4("u_m4_P", projectionMatrix);
	}

	bool OpenGLShader::HasShaderPass(const std::string& shaderPass) const
	{
		auto& iter = shaderPasses.find(shaderPass);
		if (iter != shaderPasses.end())
			return true;

		return false;
	}

	void OpenGLShader::SetTexture(const std::string& name, Ref<Texture> tex)
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

	uint32_t OpenGLShader::CreateProgram(const robin_hood::unordered_map<uint32_t, std::vector<uint32_t>>& sources)
	{
		uint32_t program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : sources)
		{
			uint32_t shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			MH_GL_CALL(glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t)));
			MH_GL_CALL(glSpecializeShader(shaderID, "main", 0, nullptr, nullptr));
			MH_GL_CALL(glAttachShader(program, shaderID));
		}

		MH_GL_CALL(glLinkProgram(program));

		GLint isLinked;
		MH_GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &isLinked));
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			MH_GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));

			std::vector<GLchar> infoLog(maxLength);
			MH_GL_CALL(glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data()));
			MH_CORE_ERROR("Shader linking failed {0}", infoLog.data());
			MH_CORE_BREAK("Error occurred while linking shader");

			MH_GL_CALL(glDeleteProgram(program));

			for (auto id : shaderIDs)
				MH_GL_CALL(glDeleteShader(id));
		}

		for (auto id : shaderIDs)
		{
			MH_GL_CALL(glDetachShader(program, id));
			MH_GL_CALL(glDeleteShader(id));
		}

		// TODO: Reflection

		return program;
	}

	robin_hood::unordered_map<uint32_t, std::vector<uint32_t>> OpenGLShader::CompileSPIRV(const robin_hood::unordered_map<GLenum, std::string>& sources, const std::string& directives)
	{
		robin_hood::unordered_map<uint32_t, std::vector<uint32_t>> compiledShaderStages;

		// Get file-friendly name
		std::string identifier = directives;
		size_t start_pos = 0;
		while ((start_pos = identifier.find(";", start_pos)) != std::string::npos) {
			identifier.replace(start_pos, 1, "_");
			start_pos += 1;
		}

		// Create shader compiler
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		for (auto& kv : sources)
		{
			const std::string filepath = "cache/shaders/" + name + identifier + GLShaderStageToFileExtension(kv.first);

			if (!std::filesystem::exists(filepath))
			{
				// Split definitions into the form: "#define X"
				std::stringstream definitions;
				if (directives.size() > 0)
				{
					std::string delim = ";";
					size_t start = 0;
					size_t end = directives.find(delim);
					while (end != std::string::npos)
					{
						definitions << "#define " << directives.substr(start, end - start) << std::endl;
						start = end + delim.length();
						end = directives.find(delim, start);
					}

					definitions << "#define " << directives.substr(start, end) << std::endl;
				}

				// Get source code with definitions
				std::string original = SortIncludes(kv.second);
				size_t firstNewline = original.find("\n") + 1;
				std::string version = original.substr(0, firstNewline);
				std::string body = original.substr(firstNewline, original.size());
				std::string source = version + definitions.str() + body;

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(kv.first), filepath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					MH_CORE_ERROR("{0}\r\n\r\n{1}\r\n\r\n{2}", source, directives, module.GetErrorMessage());
					MH_CORE_BREAK("Error occurred while compiling shader!");
				}

				compiledShaderStages[kv.first] = std::vector<uint32_t>(module.cbegin(), module.cend());

				// Write to shader cache
				std::ofstream out(filepath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = compiledShaderStages[kv.first];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
			else
			{
				std::ifstream in(filepath, std::ios::binary);
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = compiledShaderStages[kv.first];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
		}

		return compiledShaderStages;
	}

	uint32_t OpenGLShader::CompileBinary(const std::string& cachePath, const robin_hood::unordered_map<GLenum, std::string>& sources, const std::string& directives)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(sources.size() > 0, "At least one shader source is required to compile!");

		GLuint program = glCreateProgram();

		if (!std::filesystem::exists(cachePath))
		{
			// Create shader stages
			std::vector<GLuint> shaderIDs;
			shaderIDs.reserve(sources.size());
			for (auto& kv : sources)
			{
				std::string original = SortIncludes(kv.second);
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
				MH_CORE_ASSERT(false, "Shader failed to link!");

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
				MH_CORE_ASSERT(false, "Shader failed to link!");

				return 0;
			}
		}


		// Shader reflection
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

	void OpenGLShader::ParseYAMLFile(const std::string& filepath, const std::vector<std::string>& keywords)
	{
		YAML::Node rootNode;
		try
		{
			rootNode = YAML::LoadFile(filepath);
		}
		catch (YAML::Exception e)
		{
			MH_CORE_BREAK(e.what());
		}

		MH_CORE_ASSERT(rootNode && rootNode.size() > 0, "Loaded empty shader file! Path may be wrong!");

		robin_hood::unordered_map<std::string, std::string> keywordPermutations = ParseShaderKeywords(keywords);

		for each (auto shaderPassNode in rootNode)
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

			std::string cachePath = "cache/shaders/" + name + "_" + shaderPassName;

			// Read and compile include files
			auto includesNode = shaderPassNode.second["Includes"];
			if (includesNode)
			{
				// Read and parse source files
				std::stringstream source;
				for each (auto includeNode in includesNode)
				{
					std::string shaderPath = includeNode.as<std::string>();

					source << ReadFile(shaderPath);
				}

				auto sources = ParseGLSLFile(source.str());

				// Generate shaders for each shader pass * each keyword combination
				for (const auto& directives : keywordPermutations)
					shaderPasses[shaderPassName][directives.first] = CompileBinary(cachePath + directives.first + ".dat", sources, shaderPassDefines.str() + directives.second);
			}
		}
	}

	robin_hood::unordered_map<std::string, std::string> OpenGLShader::ParseShaderKeywords(const std::vector<std::string>& keywords)
	{
		const uint64_t length = keywords.size();
		const uint64_t bits = 1ULL << length;

		robin_hood::unordered_map<std::string, std::string> result;
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

	robin_hood::unordered_map<GLenum, std::string> OpenGLShader::ParseGLSLFile(const std::string& source)
	{
		MH_PROFILE_FUNCTION();

		robin_hood::unordered_map<GLenum, std::string> sources;

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

	std::string OpenGLShader::SortIncludes(const std::string& source)
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

			std::string includeSource = ReadFile(includeFile);

			sourceStream << SortIncludes(includeSource);
		}

		sourceStream << source.substr(lastPos, source.size() - lastPos);

		return sourceStream.str();
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
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

	int OpenGLShader::GetUniformLocation(const std::string& name)
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