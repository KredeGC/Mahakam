#pragma once

#include "ShaderDataTypes.h"
#include "Texture.h"

#include <string>

#include <glm/glm.hpp>

namespace Mahakam
{
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
		float Min = -std::numeric_limits<float>::infinity();
		float Max = std::numeric_limits<float>::infinity();
		std::string DefaultString;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind(const std::string& shaderPass, const std::string& variant = "") = 0;

		virtual const std::filesystem::path& GetFilepath() const = 0;
		virtual const std::string& GetName() const = 0;

		virtual const std::unordered_map<std::string, ShaderProperty>& GetProperties() const = 0;

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