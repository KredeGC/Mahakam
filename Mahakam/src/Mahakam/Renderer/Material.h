#pragma once

#include "Mahakam/Asset/Asset.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <string>

namespace Mahakam
{
	class Shader;
	class Texture;
	class UniformBuffer;

	class Material
	{
	public:
		virtual ~Material() = default;

		virtual uint64_t Hash() const = 0;

		virtual Asset<Shader> GetShader() const = 0;

		virtual void BindShader(const std::string& shaderPass) = 0;
		virtual void Bind(Ref<UniformBuffer> uniformBuffer) = 0;

		virtual void SetTexture(const std::string& name, int slot, Asset<Texture> tex) = 0;

		virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void SetInt(const std::string& name, int32_t value) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;


		virtual Asset<Texture> GetTexture(const std::string& name) const = 0;

		virtual glm::mat3 GetMat3(const std::string& name) const = 0;
		virtual glm::mat4 GetMat4(const std::string& name) const = 0;

		virtual int32_t GetInt(const std::string& name) const = 0;

		virtual float GetFloat(const std::string& name) const = 0;
		virtual glm::vec2 GetFloat2(const std::string& name) const = 0;
		virtual glm::vec3 GetFloat3(const std::string& name) const = 0;
		virtual glm::vec4 GetFloat4(const std::string& name) const = 0;


		inline static Asset<Material> Create(Asset<Shader> shader) { return CreateImpl(std::move(shader)); }
		MH_DECLARE_FUNC(Copy, Asset<Material>, Asset<Material> material);

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<Material>, Asset<Shader> shader);
	};
}