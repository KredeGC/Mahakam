#include "mhpch.h"
#include "MaterialAssetImporter.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Math/Math.h"

#include <imgui.h>

namespace Mahakam
{
	template<typename T>
	static void AddProperties(YAML::Emitter& emitter, const char* name, const std::unordered_map<std::string, T>& values)
	{
		emitter << YAML::Key << name;
		emitter << YAML::Value << YAML::BeginMap;

		for (auto& kv : values)
		{
			emitter << YAML::Key << kv.first;
			emitter << YAML::Value << kv.second;
		}

		emitter << YAML::EndMap;
	}

	MaterialAssetImporter::MaterialAssetImporter()
	{
		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}

	void MaterialAssetImporter::OnWizardOpen(YAML::Node& node)
	{
		YAML::Node shaderNode = node["Shader"];
		if (shaderNode)
		{
			Asset<Shader> shader = Asset<Shader>(shaderNode.as<uint64_t>());

			if (shader)
			{
				m_ShaderFilepath = AssetDatabase::GetAssetImportPath(shader.GetID()).string();
				SetupProperties(shader->GetProperties());
			}
		}
	}

	void MaterialAssetImporter::OnWizardRender()
	{
		char filepathBuffer[256]{ 0 };
		strncpy(filepathBuffer, m_ShaderFilepath.c_str(), m_ShaderFilepath.size());
		if (ImGui::InputText("Shader Filepath", filepathBuffer, 256))
		{
			m_ShaderFilepath = std::string(filepathBuffer);

			Asset<Shader> shader = Asset<Shader>(m_ShaderFilepath);

			if (shader)
				SetupProperties(shader->GetProperties());
		}

		for (auto& kv : m_Textures)
		{

		}

		for (auto& kv : m_Floats)
			ImGui::DragFloat(kv.first.c_str(), &kv.second, 0.01f);

		for (auto& kv : m_Float3s)
			ImGui::ColorEdit3(kv.first.c_str(), glm::value_ptr(kv.second), ImGuiColorEditFlags_HDR);
	}

	void MaterialAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Shader> shader = Asset<Shader>(m_ShaderFilepath);

		Asset<Material> material = Material::Create(shader);

		material.Save("test.material", importPath);
	}

	void MaterialAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		Asset<Material> material(asset);

		emitter << YAML::Key << "Shader";
		emitter << YAML::Value << material->GetShader().GetID();

		// Textures
		emitter << YAML::Key << "Textures";
		emitter << YAML::Value << YAML::BeginMap;

		for (auto& kv : m_Textures)
		{
			uint64_t textureID = kv.second.GetID();
			if (textureID)
			{
				emitter << YAML::Key << kv.first;
				emitter << YAML::Value << textureID;
			}
		}

		emitter << YAML::EndMap;

		// Floats
		AddProperties<float>(emitter, "Floats", m_Floats);

		// Float2s
		AddProperties<glm::vec2>(emitter, "Float2s", m_Float2s);

		// Float3s
		AddProperties<glm::vec3>(emitter, "Float3s", m_Float3s);

		// Float4s
		AddProperties<glm::vec4>(emitter, "Float4s", m_Float4s);
	}

	Asset<void> MaterialAssetImporter::Deserialize(YAML::Node& node)
	{
		Asset<Shader> shader;
		YAML::Node shaderNode = node["Shader"];
		if (shaderNode)
			shader = Asset<Shader>(shaderNode.as<uint64_t>());

		Asset<Material> material = Material::Create(shader);

		// Floats
		YAML::Node floatsNode = node["Floats"];
		if (floatsNode)
		{
			for (auto node : floatsNode)
			{
				std::string propertyName = node.first.as<std::string>();
				material->SetFloat(propertyName, node.as<float>());
			}
		}

		return material;
	}

	void MaterialAssetImporter::SetupProperties(const std::unordered_map<std::string, ShaderElement>& properties)
	{
		m_Floats.clear();
		m_Float2s.clear();
		m_Float3s.clear();
		m_Float4s.clear();

		m_Mat3s.clear();
		m_Mat4s.clear();

		m_Ints.clear();

		m_Textures.clear();

		for (auto& kv : properties)
		{
			switch (kv.second.dataType)
			{
			case ShaderDataType::Float:			m_Floats[kv.first] = 0.0f; break;
			case ShaderDataType::Float2:		m_Float2s[kv.first] = glm::vec2{ 0.0f }; break;
			case ShaderDataType::Float3:		m_Float3s[kv.first] = glm::vec3{ 0.0f }; break;
			case ShaderDataType::Float4:		m_Float4s[kv.first] = glm::vec4{ 0.0f }; break;
			case ShaderDataType::Mat3:			m_Mat3s[kv.first] = glm::mat3{ 0.0f }; break;
			case ShaderDataType::Mat4:			m_Mat4s[kv.first] = glm::mat4{ 0.0f }; break;
			case ShaderDataType::Int:			m_Ints[kv.first] = 0; break;
			case ShaderDataType::Sampler2D:		m_Textures[kv.first] = GL::GetTexture2DWhite(); break;
			case ShaderDataType::SamplerCube:	m_Textures[kv.first] = GL::GetTextureCubeWhite(); break;
			}
		}
	}
}