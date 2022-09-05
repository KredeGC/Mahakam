#pragma once

#include "ShaderDataTypes.h"

#include "Mahakam/Asset/Asset.h"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Shader;
	class Texture;

	extern template class Asset<Shader>;

	enum class ShaderPropertyType
	{
		Color,
		HDR,
		Vector,
		Range,
		Drag,
		Texture,
		Normal,
		Default
	};

	struct ShaderProperty
	{
		ShaderPropertyType PropertyType;
		ShaderDataType DataType;
		float Min = 0;
		float Max = 0;
		std::string DefaultString;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind(const std::string& shaderPass, const std::string& variant = "") = 0;

		virtual const std::filesystem::path& GetFilepath() const = 0;
		virtual const std::string& GetName() const = 0;

		virtual const UnorderedMap<std::string, ShaderProperty>& GetProperties() const = 0;

		virtual bool HasShaderPass(const std::string& shaderPass) const = 0;

		virtual void SetTexture(const std::string& name, Asset<Texture> tex) = 0;

		virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0;
		 
		virtual void SetUniformInt(const std::string& name, int value) = 0;
		 
		virtual void SetUniformFloat(const std::string& name, float value) = 0;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

		inline static Asset<Shader> Create(const std::filesystem::path& filepath, const std::initializer_list<std::string>& keywords = {}) { return CreateFilepath(filepath, keywords); }

	private:
		MH_DECLARE_FUNC(CreateFilepath, Asset<Shader>, const std::filesystem::path& filepath, const std::initializer_list<std::string>& keywords);
	};
}