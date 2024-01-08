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

		UnorderedMap<std::string, uint32_t> m_ShaderPasses;

		UnorderedMap<std::string, int> m_UniformIDCache;
		UnorderedMap<std::string, ShaderProperty> m_Properties;

		uint32_t m_UniformSize = 0;

	public:
		OpenGLShader(ShaderData&& data);
		OpenGLShader(const std::filesystem::path& filepath);

		OpenGLShader(const OpenGLShader&) = delete;
		OpenGLShader(OpenGLShader&& other) noexcept;

		virtual ~OpenGLShader();

		OpenGLShader& operator=(const OpenGLShader&) = delete;
		OpenGLShader& operator=(OpenGLShader&& rhs) noexcept;

		virtual void Bind(const std::string& shaderPass) override;

		virtual const std::filesystem::path& GetFilepath() const override { return m_Filepath; }
		virtual const std::string& GetName() const override { return m_Name; }

		virtual const UnorderedMap<std::string, ShaderProperty>& GetProperties() const override { return m_Properties; }

		virtual bool HasShaderPass(const std::string& shaderPass) const override;

		virtual void SetTexture(const std::string& name, Texture& tex) override;

		virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) override;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
		 
		virtual void SetUniformInt(const std::string& name, int value) override;
		 
		virtual void SetUniformFloat(const std::string& name, float value) override;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) override;

		uint32_t GetUniformSize() const { return m_UniformSize; }

	private:
		uint32_t CompileBinary(const UnorderedMap<ShaderStage, std::vector<uint32_t>>& spirv);

		int GetUniformLocation(const std::string& name);
	};
}