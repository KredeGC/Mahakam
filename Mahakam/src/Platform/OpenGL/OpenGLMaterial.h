#pragma once

#include "Mahakam/Renderer/Material.h"

#include "OpenGLShader.h"

namespace Mahakam
{
	class OpenGLMaterial;

	extern template class Asset<OpenGLMaterial>;

	class OpenGLMaterial : public Material
	{
	private:
		Asset<OpenGLShader> m_Shader;

		UnorderedMap<std::string, Asset<Texture>> m_Textures;
		
		UnorderedMap<std::string, glm::mat3> m_Mat3s;
		UnorderedMap<std::string, glm::mat4> m_Mat4s;
		
		UnorderedMap<std::string, int32_t> m_Ints;
		
		UnorderedMap<std::string, float> m_Floats;
		UnorderedMap<std::string, glm::vec2> m_Float2s;
		UnorderedMap<std::string, glm::vec3> m_Float3s;
		UnorderedMap<std::string, glm::vec4> m_Float4s;

	public:
		OpenGLMaterial(Asset<Shader> shader);
		OpenGLMaterial(const Asset<Material>& material);
		virtual ~OpenGLMaterial() = default;

		virtual uint64_t Hash() const override;

		inline virtual Asset<Shader> GetShader() const override { return m_Shader; }

		virtual void BindShader(const std::string& shaderPass) override { m_Shader->Bind(shaderPass); }
		virtual void Bind() override;

		virtual void SetTransform(const glm::mat4& modelMatrix) override;

		virtual void SetTexture(const std::string& name, int slot, Asset<Texture> tex) override { m_Textures[name] = tex; }

		virtual void SetMat3(const std::string& name, const glm::mat3& value) override { m_Mat3s[name] = value; }
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override { m_Mat4s[name] = value; }

		virtual void SetInt(const std::string& name, int32_t value) override { m_Ints[name] = value; }

		virtual void SetFloat(const std::string& name, float value) override { m_Floats[name] = value; }
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override { m_Float2s[name] = value; }
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override { m_Float3s[name] = value; }
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override { m_Float4s[name] = value; }


		virtual Asset<Texture> GetTexture(const std::string& name) const override;

		virtual glm::mat3 GetMat3(const std::string& name) const override;
		virtual glm::mat4 GetMat4(const std::string& name) const override;

		virtual int32_t GetInt(const std::string& name) const override;

		virtual float GetFloat(const std::string& name) const override;
		virtual glm::vec2 GetFloat2(const std::string& name) const override;
		virtual glm::vec3 GetFloat3(const std::string& name) const override;
		virtual glm::vec4 GetFloat4(const std::string& name) const override;

	private:
		void ResetShaderProperties(const UnorderedMap<std::string, ShaderProperty>& properties);
	};
}