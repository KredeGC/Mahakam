#pragma once

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/ShaderProps.h"

namespace Mahakam
{
	class HeadlessMaterial : public Material
	{
	private:
		Asset<Shader> m_Shader;

		UnorderedMap<std::string, Asset<Texture>> m_Textures;
		
		UnorderedMap<std::string, glm::mat3> m_Mat3s;
		UnorderedMap<std::string, glm::mat4> m_Mat4s;
		
		UnorderedMap<std::string, int32_t> m_Ints;
		
		UnorderedMap<std::string, float> m_Floats;
		UnorderedMap<std::string, glm::vec2> m_Float2s;
		UnorderedMap<std::string, glm::vec3> m_Float3s;
		UnorderedMap<std::string, glm::vec4> m_Float4s;

	public:
		HeadlessMaterial(Asset<Shader> shader);
		HeadlessMaterial(const Asset<Material>& material);
		virtual ~HeadlessMaterial();

		virtual uint64_t Hash() const override;

		inline virtual Asset<Shader> GetShader() const override { return m_Shader; }

		virtual void BindShader(const std::string& shaderPass) override;
		virtual void Bind(Ref<UniformBuffer> uniformBuffer) override;

		virtual void SetTexture(const std::string& name, int slot, Asset<Texture> tex) override { m_Textures[name] = std::move(tex); }

		virtual void SetMat3(const std::string& name, const glm::mat3& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual void SetInt(const std::string& name, int32_t value) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;


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