#pragma once

#include "ShaderDataTypes.h"
#include "Texture.h"

#include <string>

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

		virtual void bind(const std::string& shaderPass, const std::string& variant = "") = 0;

		virtual void setViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;

		virtual const std::string& getName() const = 0;

		virtual const std::vector<ShaderElement>& getProperties() const = 0;

		virtual bool HasShaderPass(const std::string& shaderPass) const = 0;

		virtual void setTexture(const std::string& name, Ref<Texture> tex) = 0;

		virtual void setUniformMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void setUniformMat4(const std::string& name, const glm::mat4& value) = 0;
		 
		virtual void setUniformInt(const std::string& name, int value) = 0;
		 
		virtual void setUniformFloat(const std::string& name, float value) = 0;
		virtual void setUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void setUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void setUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

		static Ref<Shader> create(const std::string& filepath, const std::initializer_list<std::string>& keywords = {});
	};


#pragma region ShaderLibrary
	class ShaderLibrary
	{
	private:
		static std::unordered_map<std::string, Ref<Shader>> shaders;

	public:
		static Ref<Shader> load(const std::string& filepath, const std::initializer_list<std::string>& defines = { "" });

		static Ref<Shader> get(const std::string& name);
	};
#pragma endregion
}