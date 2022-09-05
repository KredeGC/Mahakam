#pragma once

#include "Mahakam/Renderer/Shader.h"

namespace Mahakam
{
	class OpenGLShader : public Shader
	{
	private:
		uint32_t m_RendererID;
		std::filesystem::path m_Filepath;
		std::string m_Name;

		UnorderedMap<std::string, // Render passes
			UnorderedMap<std::string, // Shader variants
			uint32_t>> m_ShaderPasses;

		UnorderedMap<std::string, int> m_UniformIDCache;
		UnorderedMap<std::string, ShaderProperty> m_Properties;

	public:
		OpenGLShader(const std::filesystem::path& filepath, const std::initializer_list<std::string>& defines = {});
		virtual ~OpenGLShader();

		virtual void Bind(const std::string& shaderPass, const std::string& variant = "") override;

		virtual const std::filesystem::path& GetFilepath() const override { return m_Filepath; }
		virtual const std::string& GetName() const override { return m_Name; }

		virtual const UnorderedMap<std::string, ShaderProperty>& GetProperties() const override { return m_Properties; }

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
		uint32_t CompileBinary(const std::filesystem::path& cachePath, const UnorderedMap<uint32_t, std::string>& sources, const std::string& directives);

		std::string ParseDefaultValue(const YAML::Node& node);
		void ParseYAMLFile(const std::filesystem::path& filepath, const std::vector<std::string>& keywords);
		UnorderedMap<std::string, std::string> ParseShaderKeywords(const std::vector<std::string>& keywords);
		UnorderedMap<uint32_t, std::string> ParseGLSLFile(const std::string& source);

		int GetUniformLocation(const std::string& name);
	};
}