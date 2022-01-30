#pragma once

#include "Mahakam/Renderer/Shader.h"

#include <unordered_map>

#include <glm/glm.hpp>

#include <glad/glad.h>

namespace Mahakam
{
	class OpenGLShader : public Shader
	{
	private:
		uint32_t rendererID;
		std::string name;

		std::unordered_map<std::string, uint32_t> shaderVariants;
		std::unordered_map<std::string, int> uniformIDCache;
		ShaderProps properties;

	public:
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		OpenGLShader(const std::string& filepath, const std::initializer_list<std::string>& defines = { "" });
		virtual ~OpenGLShader();

		virtual void bind(const std::string& variant = "") override;

		virtual void setViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override; // Remove??

		virtual const std::string& getName() const override { return name; }

		virtual const std::vector<ShaderElement>& getProperties() const { return properties.elements; }

		virtual void setTexture(const std::string& name, Ref<Texture> tex) override;

		virtual void setUniformMat3(const std::string& name, const glm::mat3& value) override;
		virtual void setUniformMat4(const std::string& name, const glm::mat4& value) override;
		 
		virtual void setUniformInt(const std::string& name, int value) override;
		 
		virtual void setUniformFloat(const std::string& name, float value) override;
		virtual void setUniformFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void setUniformFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setUniformFloat4(const std::string& name, const glm::vec4& value) override;

	private:
		uint32_t compile(const std::unordered_map<GLenum, std::string>& sources, const std::string& directives);
		std::unordered_map<GLenum, std::string> parse(const std::string& source);
		std::string sortIncludes(const std::string& source);
		std::string readFile(const std::string& filepath);

		int getUniformLocation(const std::string& name);
	};
}