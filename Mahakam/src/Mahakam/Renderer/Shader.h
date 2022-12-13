#pragma once

#include "ShaderDataTypes.h"
#include "ShaderProps.h"

#include "Mahakam/Asset/Asset.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <ryml/rapidyaml-0.4.1.hpp>

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Texture;

	class Shader
	{
	public:
		struct SourceDefinition
		{
			UnorderedMap<ShaderStage, std::string> Sources;
			std::string Defines;
		};

		virtual ~Shader() = default;

		virtual void Bind(const std::string& shaderPass) = 0;

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

		static UnorderedMap<ShaderStage, std::string> ParseGLSLFile(const std::string& source);
		static std::string ParseDefaultValue(const ryml::NodeRef& node);
		static bool ParseYAMLFile(const std::filesystem::path& filepath, UnorderedMap<std::string, SourceDefinition>& sources, UnorderedMap<std::string, ShaderProperty>& properties);
		static bool CompileSPIRV(UnorderedMap<ShaderStage, std::vector<uint32_t>>& spirv, const SourceDefinition& source);

		inline static Asset<Shader> Create(const std::filesystem::path& filepath) { return CreateFilepath(filepath); }

	private:
		MH_DECLARE_FUNC(CreateFilepath, Asset<Shader>, const std::filesystem::path& filepath);
	};
}