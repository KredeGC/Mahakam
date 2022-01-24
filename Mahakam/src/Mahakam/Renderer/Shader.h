#pragma once

#include "ShaderDataTypes.h"

#include <string>

namespace Mahakam
{
	struct ShaderProps
	{
		ShaderDataType dataType;
		std::string name;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void bindBuffer(const std::string& name, int slot = 0) = 0; // TODO: Remove

		virtual void setViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) = 0;

		virtual const std::string& getName() const = 0;

		//virtual const ShaderProps& getProperty() const = 0;
		virtual const std::vector<ShaderProps>& getProperties() const = 0;

		static Ref<Shader> create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSrouce);
		static Ref<Shader> create(const std::string& filepath);
	};


#pragma region ShaderLibrary
	class ShaderLibrary
	{
	private:
		std::unordered_map<std::string, Ref<Shader>> shaders;

	public:
		void add(const Ref<Shader>& shader);
		Ref<Shader> load(const std::string& filepath);
		Ref<Shader> load(const std::string& name, const std::string& filepath);

		Ref<Shader> get(const std::string& name);
	};
#pragma endregion
}