#pragma once

#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	class Material
	{
	public:
		virtual ~Material() = default;

		virtual uint64_t Hash() const = 0;

		virtual Ref<Shader> GetShader() const = 0;

		virtual void BindShader(const std::string& shaderPass) const = 0;
		virtual void BindShader(const std::string& shaderPass, const std::string& variant) const = 0;
		virtual void Bind() const = 0;

		virtual void SetTransform(const glm::mat4& modelMatrix) = 0;

		virtual void SetTexture(const std::string& name, int slot, Ref<Texture> tex) = 0;

		virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void SetInt(const std::string& name, int32_t value) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;


		virtual Ref<Texture> GetTexture(const std::string& name) const = 0;

		virtual const glm::mat3 GetMat3(const std::string& name) const = 0;
		virtual const glm::mat4 GetMat4(const std::string& name) const = 0;

		virtual int32_t GetInt(const std::string& name) const = 0;

		virtual float GetFloat(const std::string& name) const = 0;
		virtual const glm::vec2 GetFloat2(const std::string& name) const = 0;
		virtual const glm::vec3 GetFloat3(const std::string& name) const = 0;
		virtual const glm::vec4 GetFloat4(const std::string& name) const = 0;

		static Ref<Material> Copy(Ref<Material> material);
		static Ref<Material> Create(Ref<Shader> shader, const std::string& variant = "");
	};
}