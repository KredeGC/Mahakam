#include "Mahakam/mhpch.h"
#include "HeadlessShader.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/Texture.h"

#include <filesystem>
#include <fstream>

namespace Mahakam
{
	HeadlessShader::HeadlessShader(const std::filesystem::path& filepath)
		: m_Filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

		// Naming
		m_Name = filepath.stem().string();

		// Read YAML file for shader passes
		UnorderedMap<std::string, SourceDefinition> sources;
		if (ParseYAMLFile(filepath, sources, m_Properties))
		{
			// Add each shader pass to the list
			for (auto& [shaderPass, sourceDef]: sources)
            {
                UnorderedMap<ShaderStage, std::vector<uint32_t>> spirv;
            
                CompileSPIRV(spirv, sourceDef);
                
				m_ShaderPasses.insert(shaderPass);
            }
		}
	}

	HeadlessShader::~HeadlessShader() {}

	void HeadlessShader::Bind(const std::string& shaderPass)
	{
		auto passIter = m_ShaderPasses.find(shaderPass);
		if (passIter == m_ShaderPasses.end())
		{
			MH_CORE_WARN("Attempted to use unknown shader pass: {0} on {1}", shaderPass, m_Name);
		}
	}

	bool HeadlessShader::HasShaderPass(const std::string& shaderPass) const
	{
		auto iter = m_ShaderPasses.find(shaderPass);
		if (iter != m_ShaderPasses.end())
			return true;

		return false;
	}

	void HeadlessShader::SetTexture(const std::string& name, Asset<Texture> tex) {}

	void HeadlessShader::SetUniformMat3(const std::string& name, const glm::mat3& value) {}

	void HeadlessShader::SetUniformMat4(const std::string& name, const glm::mat4& value) {}

	void HeadlessShader::SetUniformInt(const std::string& name, int value) {}

	void HeadlessShader::SetUniformFloat(const std::string& name, float value) {}

	void HeadlessShader::SetUniformFloat2(const std::string& name, const glm::vec2& value) {}

	void HeadlessShader::SetUniformFloat3(const std::string& name, const glm::vec3& value) {}

	void HeadlessShader::SetUniformFloat4(const std::string& name, const glm::vec4& value) {}
}