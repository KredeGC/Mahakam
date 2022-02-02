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
		std::string filepath;
		std::string name;

		std::unordered_map<std::string, // Render passes
			std::unordered_map<std::string, // Shader variants
			uint32_t>> shaderPasses;

		//std::unordered_map<std::string, uint32_t> shaderVariants;
		std::unordered_map<std::string, int> uniformIDCache;
		ShaderProps properties;

	public:
		OpenGLShader(const std::string& filepath, const std::initializer_list<std::string>& defines = {});
		virtual ~OpenGLShader();

		virtual void bind(const std::string& shaderPass, const std::string& variant = "") override;

		virtual void setViewProjection(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override; // Remove??

		virtual const std::string& getName() const override { return name; }

		virtual const std::vector<ShaderElement>& getProperties() const { return properties.elements; }

		virtual bool HasShaderPass(const std::string& shaderPass) const override;

		virtual void setTexture(const std::string& name, Ref<Texture> tex) override;

		virtual void setUniformMat3(const std::string& name, const glm::mat3& value) override;
		virtual void setUniformMat4(const std::string& name, const glm::mat4& value) override;
		 
		virtual void setUniformInt(const std::string& name, int value) override;
		 
		virtual void setUniformFloat(const std::string& name, float value) override;
		virtual void setUniformFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void setUniformFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setUniformFloat4(const std::string& name, const glm::vec4& value) override;

	private:
		// UNUSED
		uint32_t CreateProgram(const std::unordered_map<uint32_t, std::vector<uint32_t>>& sources);
		std::unordered_map<uint32_t, std::vector<uint32_t>> compile_spirv(const std::unordered_map<GLenum, std::string>& sources, const std::string& directives);
		// UNUSED

		uint32_t CompileBinary(const std::string& cachePath, const std::unordered_map<GLenum, std::string>& sources, const std::string& directives);

		void ParseYAMLFile(const std::string& filepath, const std::vector<std::string>& keywords);
		std::unordered_map<std::string, std::string> ParseShaderKeywords(const std::vector<std::string>& keywords);
		std::unordered_map<GLenum, std::string> ParseGLSLFile(const std::string& source);

		std::string sortIncludes(const std::string& source);
		std::string readFile(const std::string& filepath);

		int getUniformLocation(const std::string& name);
	};
}