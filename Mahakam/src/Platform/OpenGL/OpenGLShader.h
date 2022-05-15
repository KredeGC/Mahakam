#pragma once

#include "Mahakam/Renderer/Shader.h"

#include <robin_hood.h>

#include <glm/glm.hpp>

#include <glad/glad.h>

namespace Mahakam
{
	class OpenGLShader : public Shader
	{
	private:
		uint32_t rendererID;
		std::filesystem::path filepath;
		std::string name;

		robin_hood::unordered_map<std::string, // Render passes
			robin_hood::unordered_map<std::string, // Shader variants
			uint32_t>> shaderPasses;

		robin_hood::unordered_map<std::string, int> uniformIDCache;
		std::unordered_map<std::string, ShaderProperty> properties;

	public:
		OpenGLShader(const std::filesystem::path& filepath, const std::initializer_list<std::string>& defines = {});
		virtual ~OpenGLShader();

		virtual void Bind(const std::string& shaderPass, const std::string& variant = "") override;

		virtual const std::filesystem::path& GetFilepath() const override { return filepath; }
		virtual const std::string& GetName() const override { return name; }

		virtual const std::unordered_map<std::string, ShaderProperty>& GetProperties() const override { return properties; }

		virtual bool HasShaderPass(const std::string& shaderPass) const override;

		virtual void SetTexture(const std::string& name, Asset<Texture> tex) override;

		virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) override;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
		 
		virtual void SetUniformInt(const std::string& name, int value) override;
		 
		virtual void SetUniformFloat(const std::string& name, float value) override;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;

	private:
		uint32_t CompileBinary(const std::filesystem::path& cachePath, const robin_hood::unordered_map<GLenum, std::string>& sources, const std::string& directives);

		std::string ParseDefaultValue(YAML::Node& node);
		void ParseYAMLFile(const std::filesystem::path& filepath, const std::vector<std::string>& keywords);
		robin_hood::unordered_map<std::string, std::string> ParseShaderKeywords(const std::vector<std::string>& keywords);
		robin_hood::unordered_map<GLenum, std::string> ParseGLSLFile(const std::string& source);

		int GetUniformLocation(const std::string& name);
	};
}