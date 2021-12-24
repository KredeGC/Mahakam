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

		void compile(const std::unordered_map<GLenum, std::string>& sources);
		std::unordered_map<GLenum, std::string> parse(const std::string& source);
		std::string sortIncludes(const std::string& source);
		std::string readFile(const std::string& filepath);

	public:
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual void setViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

		virtual const std::string& getName() const override { return name; }

		void setUniformMat3(const std::string& name, const glm::mat3& value);
		void setUniformMat4(const std::string& name, const glm::mat4& value);

		void setUniformInt(const std::string& name, int value);

		void setUniformFloat(const std::string& name, float value);
		void setUniformFloat2(const std::string& name, const glm::vec2& value);
		void setUniformFloat3(const std::string& name, const glm::vec3& value);
		void setUniformFloat4(const std::string& name, const glm::vec4& value);
	};
}