#include "mhpch.h"
#include "MaterialAssetImporter.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Math/Math.h"

#include "Mahakam/ImGui/GUI.h"

#include <imgui.h>

#define MH_CONDITIONAL_COLOR(Func, Type, Setter, Getter) { Type value = m_Material->Getter(propertyName); \
	if (Func(propertyName, value, colorFlags)) \
		m_Material->Setter(propertyName, value); \
	break; }

#define MH_CONDITIONAL_SLIDER(Func, Type, Setter, Getter) { Type value = m_Material->Getter(propertyName); \
	if (Func(propertyName, value, property.Min, property.Max)) \
		m_Material->Setter(propertyName, value); \
	break; }

#define MH_CONDITIONAL_DRAG(Func, Type, Setter, Getter) { Type value = m_Material->Getter(propertyName); \
	if (Func(propertyName, value, dragSpeed, property.Min, property.Max)) \
		m_Material->Setter(propertyName, value); \
	break; }

namespace Mahakam
{
	MaterialAssetImporter::MaterialAssetImporter()
	{
		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.Extension = ".material";


		// Preview stuff
		m_PreviewSphereMesh = Mesh::CreateUVSphere(20, 20);

		m_PreviewCamera = Camera(Camera::ProjectionType::Perspective, 45, 0.03f, 10.0f);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0, 0, 2 });

		CameraData cameraData(m_PreviewCamera, { 512, 512 }, transform);

		m_CameraBuffer = UniformBuffer::Create(sizeof(CameraData));
		m_CameraBuffer->Bind();
		m_CameraBuffer->SetData(&cameraData, 0, sizeof(CameraData));

		FrameBufferProps props;
		props.width = 512;
		props.height = 512;
		props.colorAttachments = { TextureFormat::RGBA8 };
		props.depthAttachment = TextureFormat::Depth24;

		m_FrameBuffer = FrameBuffer::Create(props);
	}

	void MaterialAssetImporter::OnWizardOpen(YAML::Node& rootNode)
	{
		YAML::Node filepathNode = rootNode["Filepath"];
		if (filepathNode)
		{
			m_ShaderFilepath = filepathNode.as<std::string>();
			Asset<Shader> shader = Shader::Create(m_ShaderFilepath);

			if (shader)
			{
				m_Material = Material::Create(shader);

				SetupMaterialProperties(shader->GetProperties(), m_ShaderFilepath);

				const UnorderedMap<std::string, ShaderProperty>& properties = shader->GetProperties();
				YAML::Node propertiesNode = rootNode["Properties"];
				if (propertiesNode)
				{
					for (auto propertyNode : propertiesNode)
					{
						std::string propertyName = propertyNode.first.as<std::string>();

						auto iter = properties.find(propertyName);
						if (iter != properties.end())
						{
							switch (iter->second.DataType)
							{
							case ShaderDataType::Float:			m_Material->SetFloat(propertyName, propertyNode.second.as<float>()); break;
							case ShaderDataType::Float2:		m_Material->SetFloat2(propertyName, propertyNode.second.as<glm::vec2>()); break;
							case ShaderDataType::Float3:		m_Material->SetFloat3(propertyName, propertyNode.second.as<glm::vec3>()); break;
							case ShaderDataType::Float4:		m_Material->SetFloat4(propertyName, propertyNode.second.as<glm::vec4>()); break;
							case ShaderDataType::Mat3:			break; // TODO: Support mats
							case ShaderDataType::Mat4:			break;
							case ShaderDataType::Int:			m_Material->SetInt(propertyName, propertyNode.second.as<int>()); break;
							case ShaderDataType::Sampler2D:
							case ShaderDataType::SamplerCube:
								uint64_t textureID = propertyNode.second.as<uint64_t>();
								if (textureID)
									m_Material->SetTexture(propertyName, 0, Asset<Texture>(textureID));
								break;
							}
						}
					}
				}
			}
			else
			{
				SetupMaterialProperties({}, "");
			}
		}
		else
		{
			SetupMaterialProperties({}, "");
		}
	}

	void MaterialAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		if (filepath != m_ShaderFilepath)
		{
			m_ShaderFilepath = filepath;
			Asset<Shader> shader = Shader::Create(m_ShaderFilepath);

			if (shader)
			{
				m_Material = Material::Create(shader);
				SetupMaterialProperties(shader->GetProperties(), filepath);
			}
			else
			{
				SetupMaterialProperties({}, filepath);
			}
		}

		for (auto& kv : m_MaterialProperties)
		{
			const std::string& propertyName = kv.first;
			const ShaderProperty& property = kv.second;
			ShaderDataType dataType = property.DataType;
			ShaderPropertyType propertyType = property.PropertyType;

			float dragSpeed = glm::max(property.Max - property.Min, 1.0f) / 100.0f;
			ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_None;

			switch (propertyType)
			{
			case ShaderPropertyType::HDR: // Color edit is the same for HDR, just with some extra flags
				colorFlags |= ImGuiColorEditFlags_HDR;
			case ShaderPropertyType::Color: // Color editor
				switch (dataType)
				{
				case ShaderDataType::Float3:	MH_CONDITIONAL_COLOR(GUI::DrawColor3Edit, glm::vec3, SetFloat3, GetFloat3);
				case ShaderDataType::Float4:	MH_CONDITIONAL_COLOR(GUI::DrawColor4Edit, glm::vec4, SetFloat4, GetFloat4);
				}
				break;
			case ShaderPropertyType::Vector: // TODO: Vector visualization
				break;
			case ShaderPropertyType::Range: // Slider editor
				switch (dataType)
				{
				case ShaderDataType::Float:		MH_CONDITIONAL_SLIDER(GUI::DrawFloatSlider, float, SetFloat, GetFloat);
				case ShaderDataType::Float2:	MH_CONDITIONAL_SLIDER(GUI::DrawFloat2Slider, glm::vec2, SetFloat2, GetFloat2);
				case ShaderDataType::Float3:	MH_CONDITIONAL_SLIDER(GUI::DrawFloat3Slider, glm::vec3, SetFloat3, GetFloat3);
				case ShaderDataType::Float4:	MH_CONDITIONAL_SLIDER(GUI::DrawFloat4Slider, glm::vec4, SetFloat4, GetFloat4);
				}
				break;
			case ShaderPropertyType::Drag: // Drag editor
				switch (dataType)
				{
				case ShaderDataType::Float:		MH_CONDITIONAL_DRAG(GUI::DrawFloatDrag, float, SetFloat, GetFloat);
				case ShaderDataType::Float2:	MH_CONDITIONAL_DRAG(GUI::DrawFloat2Drag, glm::vec2, SetFloat2, GetFloat2);
				case ShaderDataType::Float3:	MH_CONDITIONAL_DRAG(GUI::DrawFloat3Drag, glm::vec3, SetFloat3, GetFloat3);
				case ShaderDataType::Float4:	MH_CONDITIONAL_DRAG(GUI::DrawFloat4Drag, glm::vec4, SetFloat4, GetFloat4);
				}
				break;
			case ShaderPropertyType::Texture: // Textures and normals are handled the same, for now
			case ShaderPropertyType::Normal:
				std::filesystem::path texturePath;
				Asset<Texture> texture = m_Material->GetTexture(propertyName);
				if (texture)
					texturePath = texture.GetImportPath();
				if (GUI::DrawDragDropTarget(propertyName, ".texture", texturePath))
				{
					texture = Asset<Texture>(texturePath);
					if (!texture)
						texture = m_DefaultTextures[propertyName];
					m_Material->SetTexture(propertyName, 0, texture);
				}

				if (texture && ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Image((void*)(uintptr_t)texture->GetRendererID(), { 128, 128 }, { 0, 1 }, { 1, 0 });
					ImGui::EndTooltip();
				}

				break;
			}
		}

		// Render a material sphere preview
		if (m_Material)
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			float maxSize = glm::min(size.x, size.y);
			ImVec2 viewportSize = { size.x, maxSize };

			m_FrameBuffer->Bind();
			GL::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			GL::Clear();

			m_PreviewCamera.SetRatio(viewportSize.x / viewportSize.y);
			m_PreviewCamera.RecalculateProjectionMatrix();
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0, 0, 2 });

			CameraData cameraData(m_PreviewCamera, { 512, 512 }, transform);

			m_CameraBuffer->Bind();
			m_CameraBuffer->SetData(&cameraData, 0, sizeof(CameraData));

			m_Material->GetShader()->Bind("GEOMETRY");
			m_Material->Bind();

			m_Material->SetTransform(glm::mat4(1.0f));

			m_PreviewSphereMesh->Bind();
			GL::DrawIndexed(m_PreviewSphereMesh->GetIndexCount());

			m_FrameBuffer->Unbind();

			ImGui::Image((void*)(uintptr_t)m_FrameBuffer->GetColorTexture(0)->GetRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
		}
	}

	void MaterialAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		m_Material.Save(filepath, importPath);

		AssetDatabase::ReloadAsset(m_Material.GetID());
	}

	void MaterialAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		Asset<Material> material(asset);

		// Material properties
		emitter << YAML::Key << "Properties";
		emitter << YAML::Value << YAML::BeginMap;

		const UnorderedMap<std::string, ShaderProperty>& properties = material->GetShader()->GetProperties();
		for (auto& kv : properties)
		{
			emitter << YAML::Key << kv.first;
			emitter << YAML::Value;

			switch (kv.second.DataType)
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
		YAML::Node shaderNode = rootNode["Filepath"];
		if (shaderNode)
			shader = Shader::Create(shaderNode.as<std::string>());

		Asset<Material> material = Material::Create(shader);

		const UnorderedMap<std::string, ShaderProperty>& properties = material->GetShader()->GetProperties();
		YAML::Node propertiesNode = rootNode["Properties"];
		if (propertiesNode)
		{
			for (auto propertyNode : propertiesNode)
			{
				std::string propertyName = propertyNode.first.as<std::string>();

				auto iter = properties.find(propertyName);
				if (iter != properties.end())
				{
					switch (iter->second.DataType)
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
						else
							material->SetTexture(propertyName, 0, GetDefaultTexture(iter->second));

						break;
					}
				}
			}
		}

		return material;
	}

	Asset<Texture> MaterialAssetImporter::GetDefaultTexture(const ShaderProperty& property)
	{
		YAML::Node rootNode;
		try
		{
			rootNode = YAML::Load(property.DefaultString);
		}
		catch (YAML::Exception e)
		{
			MH_CORE_WARN("MaterialAssetImporter encountered exception trying to import default texture: {1}", e.msg);
		}

		if (!rootNode || rootNode.size() <= 0) return nullptr;

		YAML::Node defaultNode = rootNode["Value"];
		if (defaultNode)
		{
			std::string defaultString;
			switch (property.DataType)
			{
			case ShaderDataType::Sampler2D:
				defaultString = defaultNode.as<std::string>();

				if (defaultString == "White")
					return GL::GetTexture2DWhite();
				else if (defaultString == "Black")
					return GL::GetTexture2DBlack();
				else if (defaultString == "Bump")
					return GL::GetTexture2DBump();
				else
					MH_CORE_WARN("Could not find default Texture2D of type: {0}", defaultString);

				break;
			case ShaderDataType::SamplerCube:
				defaultString = defaultNode.as<std::string>();

				if (defaultString == "White")
					return GL::GetTextureCubeWhite();
				else if (defaultString == "Black")
					return GL::GetTextureCubeWhite();
				else
					MH_CORE_WARN("Could not find default TextureCube of type: {0}", defaultString);

				break;
			}
		}

		return nullptr;
	}

	void MaterialAssetImporter::SetupMaterialProperties(const UnorderedMap<std::string, ShaderProperty>& shaderProperties, const std::filesystem::path& filepath)
	{
		m_DefaultTextures.clear();

		m_MaterialProperties = shaderProperties;


		for (auto& kv : m_MaterialProperties)
		{
			std::string propertyName = kv.first;

			YAML::Node rootNode;
			try
			{
				rootNode = YAML::Load(kv.second.DefaultString);
			}
			catch (YAML::Exception e)
			{
				MH_CORE_WARN("MaterialAssetImporter encountered exception trying to import default value {0}: {1}", propertyName, e.msg);
			}

			if (!rootNode || rootNode.size() <= 0) continue;

			YAML::Node defaultNode = rootNode["Value"];
			std::string defaultString;
			switch (kv.second.DataType)
			{
			case ShaderDataType::Float:			m_Material->SetFloat(propertyName, defaultNode.as<float>()); break;
			case ShaderDataType::Float2:		m_Material->SetFloat2(propertyName, defaultNode.as<glm::vec2>()); break;
			case ShaderDataType::Float3:		m_Material->SetFloat3(propertyName, defaultNode.as<glm::vec3>()); break;
			case ShaderDataType::Float4:		m_Material->SetFloat4(propertyName, defaultNode.as<glm::vec4>()); break;
			case ShaderDataType::Int:			m_Material->SetInt(propertyName, defaultNode.as<int>()); break;
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
				else
					MH_CORE_WARN("Could not find default Texture2D of type: {0}", defaultString);

				m_Material->SetTexture(propertyName, 0, m_DefaultTextures[propertyName]);

				break;
			case ShaderDataType::SamplerCube:
				defaultString = defaultNode.as<std::string>();

				if (defaultString == "White")
					m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();
				else if (defaultString == "Black")
					m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();
				else
					MH_CORE_WARN("Could not find default TextureCube of type: {0}", defaultString);

				m_Material->SetTexture(propertyName, 0, m_DefaultTextures[propertyName]);

				break;
			}
		}
	}
}