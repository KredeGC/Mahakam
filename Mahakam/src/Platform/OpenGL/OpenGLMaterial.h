#pragma once

#include "OpenGLShader.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	class OpenGLMaterial : public Material
	{
	private:
		Ref<OpenGLShader> shader;

		std::string variant = "";

		std::unordered_map<std::string, Ref<Texture>> textures;

		std::unordered_map<std::string, glm::mat3> mat3s;
		std::unordered_map<std::string, glm::mat4> mat4s;

		std::unordered_map<std::string, uint32_t> ints;

		std::unordered_map<std::string, float> floats;
		std::unordered_map<std::string, glm::vec2> float2s;
		std::unordered_map<std::string, glm::vec3> float3s;
		std::unordered_map<std::string, glm::vec4> float4s;

	public:
		OpenGLMaterial(const Ref<Shader>& shader, const std::string& variant = "");
		OpenGLMaterial(const Ref<Material>& material);

		inline virtual Ref<Shader> GetShader() const override { return shader; }

		virtual void BindShader(const std::string& shaderPass) const override { shader->Bind(shaderPass); }
		virtual void BindShader(const std::string& shaderPass, const std::string& variant) const override { shader->Bind(shaderPass, variant); }
		virtual void Bind() const override;

		virtual void SetTransform(const glm::mat4& modelMatrix) override;

		virtual void SetTexture(const std::string& name, int slot, Ref<Texture> tex) override { textures[name] = tex; }

		virtual void SetMat3(const std::string& name, const glm::mat3& value) override { mat3s[name] = value; }
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override { mat4s[name] = value; }

		virtual void SetInt(const std::string& name, int32_t value) override { ints[name] = value; }

		virtual void SetFloat(const std::string& name, float value) override { floats[name] = value; }
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override { float2s[name] = value; }
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override { float3s[name] = value; }
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override { float4s[name] = value; }


		virtual Ref<Texture> GetTexture(const std::string& name) const override;

		virtual const glm::mat3 GetMat3(const std::string& name) const override;
		virtual const glm::mat4 GetMat4(const std::string& name) const override;

		virtual int32_t GetInt(const std::string& name) const override;

		virtual float GetFloat(const std::string& name) const override;
		virtual const glm::vec2 GetFloat2(const std::string& name) const override;
		virtual const glm::vec3 GetFloat3(const std::string& name) const override;
		virtual const glm::vec4 GetFloat4(const std::string& name) const override;
	};
}