#include "Mahakam/mhpch.h"
#include "HeadlessShader.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/Texture.h"

#include <yaml-cpp/yaml.h>

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
		ParseYAMLFile(filepath);
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

	void HeadlessShader::SetTexture(const std::string& name, Ref<Texture> tex) {}

	void HeadlessShader::SetUniformMat3(const std::string& name, const glm::mat3& value) {}

	void HeadlessShader::SetUniformMat4(const std::string& name, const glm::mat4& value) {}

	void HeadlessShader::SetUniformInt(const std::string& name, int value) {}

	void HeadlessShader::SetUniformFloat(const std::string& name, float value) {}

	void HeadlessShader::SetUniformFloat2(const std::string& name, const glm::vec2& value) {}

	void HeadlessShader::SetUniformFloat3(const std::string& name, const glm::vec3& value) {}

	void HeadlessShader::SetUniformFloat4(const std::string& name, const glm::vec4& value) {}

	std::string HeadlessShader::ParseDefaultValue(const YAML::Node& node)
	{
		if (node)
		{
			if (node.IsScalar())
			{
				return node.Scalar();
			}
			else if (node.IsSequence())
			{
				std::stringstream value;
				for (auto iter = node.begin(); iter != node.end();)
				{
					value << ParseDefaultValue(*iter);
					if (++iter != node.end())
						value << ",";
				}
				return "[" + value.str() + "]";
			}
		}

		return "";
	}

	void HeadlessShader::ParseYAMLFile(const std::filesystem::path& filepath)
	{
		YAML::Node rootNode;
		try
		{
			rootNode = YAML::LoadFile(filepath.string());
		}
		catch (YAML::Exception e)
		{
			MH_CORE_WARN("HeadlessShader encountered exception trying to parse YAML file {0}: {1}", filepath, e.msg);
		}

		MH_CORE_ASSERT(rootNode && rootNode.size() > 0, "Loaded empty shader file! Path may be wrong!");

		// Read properties
		auto propertiesNode = rootNode["Properties"];

		MH_CORE_INFO("Loading properties for shader: {0}", m_Name);
		for (auto propertyNode : propertiesNode)
		{
			std::string propertyName = propertyNode.first.as<std::string>();

			YAML::Node typeNode = propertyNode.second["Type"];
			YAML::Node minNode = propertyNode.second["Min"];
			YAML::Node maxNode = propertyNode.second["Max"];
			YAML::Node defaultNode = propertyNode.second["Default"];

			ShaderPropertyType propertyType = ShaderPropertyType::Default;
			if (typeNode)
			{
				std::string typeString = typeNode.as<std::string>();
				if (typeString == "Color")			propertyType = ShaderPropertyType::Color;
				else if (typeString == "HDR")		propertyType = ShaderPropertyType::HDR;
				else if (typeString == "Vector")	propertyType = ShaderPropertyType::Vector;
				else if (typeString == "Range")		propertyType = ShaderPropertyType::Range;
				else if (typeString == "Drag")		propertyType = ShaderPropertyType::Drag;
				else if (typeString == "Texture")	propertyType = ShaderPropertyType::Texture;
				else if (typeString == "Normal")	propertyType = ShaderPropertyType::Normal;
				else if (typeString == "Default")	propertyType = ShaderPropertyType::Default;
			}

			float min = 0;
			if (minNode)
				min = minNode.as<float>();

			float max = 0;
			if (maxNode)
				max = maxNode.as<float>();

			std::string defaultValue = ParseDefaultValue(defaultNode);

			m_Properties[propertyName] = { propertyType, ShaderDataType::None, min, max, "Value: " + defaultValue, 1, 0 };

			MH_CORE_INFO("  {0}: {1}", propertyName, defaultValue);
		}

		// Read shader passes
		auto passesNode = rootNode["Passes"];

		for (auto shaderPassNode : passesNode)
		{
			std::string shaderPassName = shaderPassNode.first.as<std::string>();

			// Read shaderpass defines
			std::stringstream shaderPassDefines;
			auto definesNode = shaderPassNode.second["Defines"];
			if (definesNode)
			{
				for (auto define : definesNode)
					shaderPassDefines << "#define " + define.as<std::string>() + "\n";
			}

			const std::string passPath = filepath.string() + "_" + shaderPassName;

			// Read and compile include files
			auto includesNode = shaderPassNode.second["Includes"];
			if (includesNode)
			{
				m_ShaderPasses.emplace(shaderPassName);
			}
		}
	}
}