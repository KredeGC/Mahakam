#pragma once

#include "Mahakam/Asset/Asset.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	class Material;
	class Shader;
	class Texture;

	extern template class Asset<Material>;
	extern template class Asset<Shader>;
	extern template class Asset<Texture>;

	class Material
	{
	public:
		virtual ~Material() = default;

		virtual uint64_t Hash() const = 0;

		virtual Asset<Shader> GetShader() const = 0;

		virtual void BindShader(const std::string& shaderPass) = 0;
		virtual void BindShader(const std::string& shaderPass, const std::string& variant) = 0;
		virtual void Bind() = 0;

		virtual void SetTransform(const glm::mat4& modelMatrix) = 0;

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


		virtual const UnorderedMap<std::string, Asset<Texture>>& GetTextures() const = 0;

		virtual const UnorderedMap<std::string, glm::mat3>& GetMat3s() const = 0;
		virtual const UnorderedMap<std::string, glm::mat4>& GetMat4s() const = 0;

		virtual const UnorderedMap<std::string, int32_t>& GetInts() const = 0;

		virtual const UnorderedMap<std::string, float>& GetFloats() const = 0;
		virtual const UnorderedMap<std::string, glm::vec2>& GetFloat2s() const = 0;
		virtual const UnorderedMap<std::string, glm::vec3>& GetFloat3s() const = 0;
		virtual const UnorderedMap<std::string, glm::vec4>& GetFloat4s() const = 0;


		inline static Asset<Material> Create(Asset<Shader> shader, const std::string& variant = "") { return CreateImpl(shader, variant); }
		MH_DECLARE_FUNC(Copy, Asset<Material>, Asset<Material> material);

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<Material>, Asset<Shader> shader, const std::string& variant);
	};
}