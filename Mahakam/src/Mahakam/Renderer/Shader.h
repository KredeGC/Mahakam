#pragma once

#include "ShaderDataTypes.h"
#include "Texture.h"

#include <string>

#include <glm/glm.hpp>

namespace Mahakam
{
	struct ShaderElement
	{
		ShaderDataType dataType;
		std::string name;
		uint32_t location;

		ShaderElement(ShaderDataType dataType, const std::string& name, uint32_t location)
			: dataType(dataType), name(name), location(location) {}

		ShaderElement(ShaderDataType dataType, const std::string& name)
			: dataType(dataType), name(name), location(0) {}

		ShaderElement(ShaderDataType dataType, uint32_t location)
			: dataType(dataType), name(""), location(location) {}
	};

	struct ShaderProps
	{
		std::vector<ShaderElement> elements;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind(const std::string& shaderPass, const std::string& variant = "") = 0;

		virtual const std::string& GetFilepath() const = 0;
		virtual const std::string& GetName() const = 0;

		virtual const std::vector<ShaderElement>& GetProperties() const = 0;

		virtual bool HasShaderPass(const std::string& shaderPass) const = 0;

		virtual void SetTexture(const std::string& name, Ref<Texture> tex) = 0;

		virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0;
		 
		virtual void SetUniformInt(const std::string& name, int value) = 0;
		 
		virtual void SetUniformFloat(const std::string& name, float value) = 0;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

		inline static Ref<Shader> Create(const std::string& filepath, const std::initializer_list<std::string>& keywords = {}) { return CreateFilepath(filepath, keywords); }

	private:
		MH_DECLARE_FUNC(CreateFilepath, Ref<Shader>, const std::string& filepath, const std::initializer_list<std::string>& keywords);
	};
}