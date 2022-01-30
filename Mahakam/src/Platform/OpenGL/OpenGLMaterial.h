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

		inline virtual Ref<Shader> getShader() const override { return shader; }

		virtual void bindShader(const std::string& variant) const override { shader->bind(variant); }
		virtual void bindShader() const override { shader->bind(variant); }
		virtual void bind() const override;

		virtual void setTransform(const glm::mat4& modelMatrix) override;

		virtual void setTexture(const std::string& name, int slot, Ref<Texture> tex) override { textures[name] = tex; }

		virtual void setMat3(const std::string& name, const glm::mat3& value) override { mat3s[name] = value; }
		virtual void setMat4(const std::string& name, const glm::mat4& value) override { mat4s[name] = value; }

		virtual void setInt(const std::string& name, int32_t value) override { ints[name] = value; }

		virtual void setFloat(const std::string& name, float value) override { floats[name] = value; }
		virtual void setFloat2(const std::string& name, const glm::vec2& value) override { float2s[name] = value; }
		virtual void setFloat3(const std::string& name, const glm::vec3& value) override { float3s[name] = value; }
		virtual void setFloat4(const std::string& name, const glm::vec4& value) override { float4s[name] = value; }


		virtual Ref<Texture> getTexture(const std::string& name) const override;

		virtual const glm::mat3 getMat3(const std::string& name) const override;
		virtual const glm::mat4 getMat4(const std::string& name) const override;

		virtual int32_t getInt(const std::string& name) const override;

		virtual float getFloat(const std::string& name) const override;
		virtual const glm::vec2 getFloat2(const std::string& name) const override;
		virtual const glm::vec3 getFloat3(const std::string& name) const override;
		virtual const glm::vec4 getFloat4(const std::string& name) const override;
	};
}