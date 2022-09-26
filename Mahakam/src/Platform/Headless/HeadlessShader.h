#pragma once

#include "Mahakam/Renderer/Shader.h"

#include <unordered_set>

namespace Mahakam
{
	class HeadlessShader;
	class UniformBuffer;

	extern template class Asset<HeadlessShader>;

	class HeadlessShader : public Shader
	{
	private:
		std::filesystem::path m_Filepath;
		std::string m_Name;

		std::unordered_set<std::string> m_ShaderPasses;

		UnorderedMap<std::string, ShaderProperty> m_Properties;

	public:
		HeadlessShader(const std::filesystem::path& filepath);
		virtual ~HeadlessShader();

		virtual void Bind(const std::string& shaderPass) override;

		virtual const std::filesystem::path& GetFilepath() const override { return m_Filepath; }
		virtual const std::string& GetName() const override { return m_Name; }

		virtual const UnorderedMap<std::string, ShaderProperty>& GetProperties() const override { return m_Properties; }

		virtual bool HasShaderPass(const std::string& shaderPass) const override;

		virtual void SetTexture(const std::string& name, Ref<Texture> tex) override;

		virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) override;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
		 
		virtual void SetUniformInt(const std::string& name, int value) override;
		 
		virtual void SetUniformFloat(const std::string& name, float value) override;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;

	private:
		std::string ParseDefaultValue(const YAML::Node& node);
		void ParseYAMLFile(const std::filesystem::path& filepath);
	};
}