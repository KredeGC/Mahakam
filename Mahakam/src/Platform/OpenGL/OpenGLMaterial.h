#pragma once

#include "OpenGLShader.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Texture.h"

#include <robin_hood.h>

namespace Mahakam
{
	class OpenGLMaterial : public Material
	{
	private:
		Asset<OpenGLShader> shader;

		std::string variant = "";

		robin_hood::unordered_map<std::string, Asset<Texture>> textures;
		
		robin_hood::unordered_map<std::string, glm::mat3> mat3s;
		robin_hood::unordered_map<std::string, glm::mat4> mat4s;
		
		robin_hood::unordered_map<std::string, int32_t> ints;
		
		robin_hood::unordered_map<std::string, float> floats;
		robin_hood::unordered_map<std::string, glm::vec2> float2s;
		robin_hood::unordered_map<std::string, glm::vec3> float3s;
		robin_hood::unordered_map<std::string, glm::vec4> float4s;

	public:
		OpenGLMaterial(const Asset<Shader>& shader, const std::string& variant = "");
		OpenGLMaterial(const Asset<Material>& material);

		virtual uint64_t Hash() const override;

		inline virtual Asset<Shader> GetShader() const override { return shader; }

		virtual void BindShader(const std::string& shaderPass) override { shader->Bind(shaderPass); }
		virtual void BindShader(const std::string& shaderPass, const std::string& variant) override { shader->Bind(shaderPass, variant); }
		virtual void Bind() override;

		virtual void SetTransform(const glm::mat4& modelMatrix) override;

		virtual void SetTexture(const std::string& name, int slot, Asset<Texture> tex) override { textures[name] = tex; }

		virtual void SetMat3(const std::string& name, const glm::mat3& value) override { mat3s[name] = value; }
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override { mat4s[name] = value; }

		virtual void SetInt(const std::string& name, int32_t value) override { ints[name] = value; }

		virtual void SetFloat(const std::string& name, float value) override { floats[name] = value; }
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override { float2s[name] = value; }
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override { float3s[name] = value; }
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override { float4s[name] = value; }


		virtual Asset<Texture> GetTexture(const std::string& name) const override;

		virtual const glm::mat3 GetMat3(const std::string& name) const override;
		virtual const glm::mat4 GetMat4(const std::string& name) const override;

		virtual int32_t GetInt(const std::string& name) const override;

		virtual float GetFloat(const std::string& name) const override;
		virtual const glm::vec2 GetFloat2(const std::string& name) const override;
		virtual const glm::vec3 GetFloat3(const std::string& name) const override;
		virtual const glm::vec4 GetFloat4(const std::string& name) const override;


		virtual const robin_hood::unordered_map<std::string, Asset<Texture>>& GetTextures() const { return textures; }

		virtual const robin_hood::unordered_map<std::string, glm::mat3>& GetMat3s() const { return mat3s; }
		virtual const robin_hood::unordered_map<std::string, glm::mat4>& GetMat4s() const { return mat4s; }

		virtual const robin_hood::unordered_map<std::string, int32_t>& GetInts() const { return ints; }

		virtual const robin_hood::unordered_map<std::string, float>& GetFloats() const { return floats; }
		virtual const robin_hood::unordered_map<std::string, glm::vec2>& GetFloat2s() const { return float2s; }
		virtual const robin_hood::unordered_map<std::string, glm::vec3>& GetFloat3s() const { return float3s; }
		virtual const robin_hood::unordered_map<std::string, glm::vec4>& GetFloat4s() const { return float4s; }

	private:
		void ResetShaderProperties(const std::unordered_map<std::string, ShaderProperty>& properties);
	};
}