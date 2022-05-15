#include "mhpch.h"
#include "MaterialAssetImporter.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Math/Math.h"

#include "Mahakam/ImGui/GUI.h"

#include <imgui.h>

namespace Mahakam
{
	MaterialAssetImporter::MaterialAssetImporter()
	{
		m_ImporterProps.CreateMenu = true;
		//m_ImporterProps.NoFilepath = true;
	}

	void MaterialAssetImporter::OnWizardOpen(YAML::Node& rootNode)
	{
		YAML::Node filepathNode = rootNode["Filepath"];
		std::filesystem::path filepath;
		if (filepathNode)
			filepath = filepathNode.as<std::string>();

		YAML::Node shaderNode = rootNode["Shader"];
		if (shaderNode)
		{
			Asset<Shader> shader = Asset<Shader>(shaderNode.as<uint64_t>());

			if (shader)
			{
				m_ShaderFilepath = AssetDatabase::GetAssetImportPath(shader.GetID()).string();
				SetupMaterialProperties(shader->GetProperties(), filepath);

				const std::unordered_map<std::string, ShaderElement>& properties = shader->GetProperties();
				YAML::Node propertiesNode = rootNode["Properties"];
				if (propertiesNode)
				{
					for (auto propertyNode : propertiesNode)
					{
						std::string propertyName = propertyNode.first.as<std::string>();

						auto iter = properties.find(propertyName);
						if (iter != properties.end())
						{
							switch (iter->second.dataType)
							{
							case ShaderDataType::Float:			m_Floats[propertyName] = propertyNode.second.as<float>(); break;
							case ShaderDataType::Float2:		m_Float2s[propertyName] = propertyNode.second.as<glm::vec2>(); break;
							case ShaderDataType::Float3:		m_Float3s[propertyName] = propertyNode.second.as<glm::vec3>(); break;
							case ShaderDataType::Float4:		m_Float4s[propertyName] = propertyNode.second.as<glm::vec4>(); break;
							case ShaderDataType::Mat3:			break; // TODO: Support mats
							case ShaderDataType::Mat4:			break;
							case ShaderDataType::Int:			m_Ints[propertyName] = propertyNode.second.as<int>(); break;
							case ShaderDataType::Sampler2D:
							case ShaderDataType::SamplerCube:
								uint64_t textureID = propertyNode.second.as<uint64_t>();
								if (textureID)
									m_Textures[propertyName] = Asset<Texture>(textureID);
								break;
							}
						}
					}
				}
			}
			else
			{
				SetupMaterialProperties({}, filepath);
			}
		}
		else
		{
			SetupMaterialProperties({}, filepath);
		}
	}

	void MaterialAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		if (GUI::DrawDragDropTarget("Shader Filepath", ".shader", m_ShaderFilepath))
		{
			Asset<Shader> shader = Asset<Shader>(m_ShaderFilepath);

			if (shader)
				SetupMaterialProperties(shader->GetProperties(), filepath);
			else
				SetupMaterialProperties({}, filepath);
		}

		for (auto& kv : m_MaterialProperties)
		{
			const std::string& propertyName = kv.first;
			const MaterialProperty& property = kv.second;
			ShaderDataType dataType = property.DataType;
			MaterialPropertyType propertyType = property.PropertyType;
			float dragSpeed = (property.Max - property.Min) / 100.0f;
			switch (propertyType)
			{
			case MaterialPropertyType::Color:
				switch (dataType)
				{
				case ShaderDataType::Float3:	ImGui::ColorEdit3(propertyName.c_str(), glm::value_ptr(m_Float3s[propertyName])); break;
				case ShaderDataType::Float4:	ImGui::ColorEdit4(propertyName.c_str(), glm::value_ptr(m_Float4s[propertyName])); break;
				}
				break;
			case MaterialPropertyType::HDR:
				switch (dataType)
				{
				case ShaderDataType::Float3:	ImGui::ColorEdit3(propertyName.c_str(), glm::value_ptr(m_Float3s[propertyName]), ImGuiColorEditFlags_HDR); break;
				case ShaderDataType::Float4:	ImGui::ColorEdit4(propertyName.c_str(), glm::value_ptr(m_Float4s[propertyName]), ImGuiColorEditFlags_HDR); break;
				}
				break;
			case MaterialPropertyType::Vector: // TODO: Vector visualization
				break;
			case MaterialPropertyType::Range:
				switch (dataType)
				{
				case ShaderDataType::Float:		ImGui::SliderFloat(propertyName.c_str(), &m_Floats[propertyName], property.Min, property.Max); break;
				case ShaderDataType::Float2:	ImGui::SliderFloat2(propertyName.c_str(), glm::value_ptr(m_Float2s[propertyName]), property.Min, property.Max); break;
				case ShaderDataType::Float3:	ImGui::SliderFloat3(propertyName.c_str(), glm::value_ptr(m_Float3s[propertyName]), property.Min, property.Max); break;
				case ShaderDataType::Float4:	ImGui::SliderFloat4(propertyName.c_str(), glm::value_ptr(m_Float4s[propertyName]), property.Min, property.Max); break;
				}
				break;
			case MaterialPropertyType::Drag:
				switch (dataType)
				{
				case ShaderDataType::Float:		ImGui::DragFloat(propertyName.c_str(), &m_Floats[propertyName], dragSpeed, property.Min, property.Max); break;
				case ShaderDataType::Float2:	ImGui::DragFloat2(propertyName.c_str(), glm::value_ptr(m_Float2s[propertyName]), dragSpeed, property.Min, property.Max); break;
				case ShaderDataType::Float3:	ImGui::DragFloat3(propertyName.c_str(), glm::value_ptr(m_Float3s[propertyName]), dragSpeed, property.Min, property.Max); break;
				case ShaderDataType::Float4:	ImGui::DragFloat4(propertyName.c_str(), glm::value_ptr(m_Float4s[propertyName]), dragSpeed, property.Min, property.Max); break;
				}
				break;
			case MaterialPropertyType::Texture:
			case MaterialPropertyType::Normal:
				std::filesystem::path texturePath;
				if (m_Textures[propertyName])
					texturePath = m_Textures[propertyName].GetImportPath();
				if (GUI::DrawTextureDragDropTarget(propertyName, texturePath))
				{
					Asset<Texture> texture = Asset<Texture>(texturePath);
					if (!texture)
						texture = m_DefaultTextures[propertyName];
					m_Textures[propertyName] = texture;
				}

				Asset<Texture>& texture = m_Textures[propertyName];
				if (texture && ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Image((void*)(uintptr_t)texture->GetRendererID(), { 128, 128 }, { 0, 1 }, { 1, 0 });
					ImGui::EndTooltip();
				}

				break;
			}
		}
	}

	void MaterialAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Shader> shader = Asset<Shader>(m_ShaderFilepath);

		Asset<Material> material = Material::Create(shader);

		for (auto& kv : m_MaterialProperties)
		{
			switch (kv.second.DataType)
			{
			case ShaderDataType::Float:			material->SetFloat(kv.first, m_Floats[kv.first]); break;
			case ShaderDataType::Float2:		material->SetFloat2(kv.first, m_Float2s[kv.first]); break;
			case ShaderDataType::Float3:		material->SetFloat3(kv.first, m_Float3s[kv.first]); break;
			case ShaderDataType::Float4:		material->SetFloat4(kv.first, m_Float4s[kv.first]); break;
			case ShaderDataType::Mat3:			material->SetMat3(kv.first, m_Mat3s[kv.first]); break;
			case ShaderDataType::Mat4:			material->SetMat4(kv.first, m_Mat4s[kv.first]); break;
			case ShaderDataType::Int:			material->SetInt(kv.first, m_Ints[kv.first]); break;
			case ShaderDataType::Sampler2D:		material->SetTexture(kv.first, 0, m_Textures[kv.first]); break;
			case ShaderDataType::SamplerCube:	material->SetTexture(kv.first, 0, m_Textures[kv.first]); break;
			}
		}

		material.Save(filepath, importPath);

		AssetDatabase::ReloadAsset(material.GetID());
	}

	void MaterialAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		Asset<Material> material(asset);

		emitter << YAML::Key << "Shader";
		emitter << YAML::Value << material->GetShader().GetID();

		// Material properties
		emitter << YAML::Key << "Properties";
		emitter << YAML::Value << YAML::BeginMap;

		const std::unordered_map<std::string, ShaderElement>& properties = material->GetShader()->GetProperties();
		for (auto& kv : properties)
		{
			emitter << YAML::Key << kv.first;
			emitter << YAML::Value;

			switch (kv.second.dataType)
			{
			case ShaderDataType::Float:			emitter << material->GetFloat(kv.first); break;
			case ShaderDataType::Float2:		emitter << material->GetFloat2(kv.first); break;
			case ShaderDataType::Float3:		emitter << material->GetFloat3(kv.first); break;
			case ShaderDataType::Float4:		emitter << material->GetFloat4(kv.first); break;
			case ShaderDataType::Mat3:			emitter << material->GetMat3(kv.first); break;
			case ShaderDataType::Mat4:			emitter << material->GetMat4(kv.first); break;
			case ShaderDataType::Int:			emitter << material->GetInt(kv.first); break;
			case ShaderDataType::Sampler2D:		emitter << material->GetTexture(kv.first).GetID(); break;
			case ShaderDataType::SamplerCube:	emitter << material->GetTexture(kv.first).GetID(); break;
			}
		}

		emitter << YAML::EndMap;
	}

	Asset<void> MaterialAssetImporter::Deserialize(YAML::Node& rootNode)
	{
		Asset<Shader> shader;
		YAML::Node shaderNode = rootNode["Shader"];
		if (shaderNode)
			shader = Asset<Shader>(shaderNode.as<uint64_t>());

		Asset<Material> material = Material::Create(shader);

		const std::unordered_map<std::string, ShaderElement>& properties = material->GetShader()->GetProperties();
		YAML::Node propertiesNode = rootNode["Properties"];
		if (propertiesNode)
		{
			for (auto propertyNode : propertiesNode)
			{
				std::string propertyName = propertyNode.first.as<std::string>();

				auto iter = properties.find(propertyName);
				if (iter != properties.end())
				{
					switch (iter->second.dataType)
					{
					case ShaderDataType::Float:			material->SetFloat(propertyName, propertyNode.second.as<float>()); break;
					case ShaderDataType::Float2:		material->SetFloat2(propertyName, propertyNode.second.as<glm::vec2>()); break;
					case ShaderDataType::Float3:		material->SetFloat3(propertyName, propertyNode.second.as<glm::vec3>()); break;
					case ShaderDataType::Float4:		material->SetFloat4(propertyName, propertyNode.second.as<glm::vec4>()); break;
						//case ShaderDataType::Mat3:			material->SetMat3(propertyName, propertyNode.second.as<glm::mat3>()); break;
						//case ShaderDataType::Mat4:			material->SetMat4(propertyName, propertyNode.second.as<glm::mat4>()); break;
					case ShaderDataType::Int:			material->SetInt(propertyName, propertyNode.second.as<int>()); break;
					case ShaderDataType::Sampler2D:
					case ShaderDataType::SamplerCube:
						uint64_t textureID = propertyNode.second.as<uint64_t>();
						if (textureID)
							material->SetTexture(propertyName, 0, Asset<Texture>(textureID));
						break;
					}
				}
			}
		}

		return material;
	}

	void MaterialAssetImporter::SetupMaterialProperties(const std::unordered_map<std::string, ShaderElement>& shaderProperties, const std::filesystem::path& filepath)
	{
		m_Floats.clear();
		m_Float2s.clear();
		m_Float3s.clear();
		m_Float4s.clear();

		m_Mat3s.clear();
		m_Mat4s.clear();

		m_Ints.clear();

		m_DefaultTextures.clear();
		m_Textures.clear();

		m_MaterialProperties.clear();

		YAML::Node rootNode;
		if (std::filesystem::exists(filepath))
		{
			try
			{
				rootNode = YAML::LoadFile(filepath.string());
			}
			catch (YAML::Exception e)
			{
				MH_CORE_WARN("MaterialAssetImporter encountered exception trying to import yaml file {0}: {1}", filepath, e.msg);
			}
		}

		if (!rootNode || rootNode.size() <= 0) return;

		YAML::Node propertiesNode = rootNode["Properties"];
		if (propertiesNode)
		{
			for (auto propertyNode : propertiesNode)
			{
				std::string propertyName = propertyNode.first.as<std::string>();

				auto iter = shaderProperties.find(propertyName);
				if (iter != shaderProperties.end())
				{
					YAML::Node typeNode = propertyNode.second["Type"];
					YAML::Node minNode = propertyNode.second["Min"];
					YAML::Node maxNode = propertyNode.second["Max"];
					YAML::Node defaultNode = propertyNode.second["Default"];

					MaterialPropertyType propertyType = MaterialPropertyType::Default;
					if (typeNode)
					{
						std::string typeString = typeNode.as<std::string>();
						if (typeString == "Color")			propertyType = MaterialPropertyType::Color;
						else if (typeString == "HDR")		propertyType = MaterialPropertyType::HDR;
						else if (typeString == "Vector")	propertyType = MaterialPropertyType::Vector;
						else if (typeString == "Range")		propertyType = MaterialPropertyType::Range;
						else if (typeString == "Drag")		propertyType = MaterialPropertyType::Drag;
						else if (typeString == "Texture")	propertyType = MaterialPropertyType::Texture;
						else if (typeString == "Normal")	propertyType = MaterialPropertyType::Normal;
						else if (typeString == "Default")	propertyType = MaterialPropertyType::Default;
					}

					float min = -std::numeric_limits<float>::infinity();
					if (minNode)
						min = minNode.as<float>();

					float max = std::numeric_limits<float>::infinity();
					if (maxNode)
						max = maxNode.as<float>();

					m_MaterialProperties[propertyName] = { propertyType, iter->second.dataType, min, max };

					if (defaultNode)
					{
						std::string defaultString;
						switch (iter->second.dataType)
						{
						case ShaderDataType::Float:			m_Floats[propertyName] = defaultNode.as<float>(); break;
						case ShaderDataType::Float2:		m_Float2s[propertyName] = defaultNode.as<glm::vec2>(); break;
						case ShaderDataType::Float3:		m_Float3s[propertyName] = defaultNode.as<glm::vec3>(); break;
						case ShaderDataType::Float4:		m_Float4s[propertyName] = defaultNode.as<glm::vec4>(); break;
						case ShaderDataType::Int:			m_Ints[propertyName] = defaultNode.as<int>(); break;
						case ShaderDataType::Mat3:			break; // TODO: Support these types
						case ShaderDataType::Mat4:			break;
						case ShaderDataType::Sampler2D:
							defaultString = defaultNode.as<std::string>();

							if (defaultString == "White")
								m_DefaultTextures[propertyName] = GL::GetTexture2DWhite();
							else if (defaultString == "Black")
								m_DefaultTextures[propertyName] = GL::GetTexture2DBlack();
							else if (defaultString == "Bump")
								m_DefaultTextures[propertyName] = GL::GetTexture2DBump();

							m_Textures[propertyName] = m_DefaultTextures[propertyName];

							break;
						case ShaderDataType::SamplerCube:
							defaultString = defaultNode.as<std::string>();

							if (defaultString == "White")
								m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();
							else if (defaultString == "Black")
								m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();

							m_Textures[propertyName] = m_DefaultTextures[propertyName];

							break;
						}
					}
				}
			}
		}
	}
}