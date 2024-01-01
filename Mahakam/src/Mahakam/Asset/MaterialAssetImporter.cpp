#include "Mahakam/mhpch.h"
#include "MaterialAssetImporter.h"

#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/Log.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/RenderPasses/GeometryRenderPass.h"
#include "Mahakam/Renderer/RenderPasses/LightingRenderPass.h"
#include "Mahakam/Renderer/RenderPasses/TonemappingRenderPass.h"
#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/Camera.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Light.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

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

#define MH_CONDITIONAL_SET_VALUE(Func, Type) { Type value; \
	propertyNode >> value; \
	Func(propertyName, value); \
	break; }

namespace Mahakam
{
	MaterialAssetImporter::MaterialAssetImporter()
	{
		Setup(m_ImporterProps, "Material", ".material");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;


#ifndef MH_STANDALONE
		// Preview stuff
		m_PreviewSphereMesh = SubMesh::CreateUVSphere(40, 20);

		m_PreviewCamera = Camera(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.03f, 5.0f);

		// Setup scene data
		m_SceneData = CreateRef<SceneData>();
		m_SceneData->MeshIDLookup[0] = m_PreviewSphereMesh;
		m_SceneData->MeshRefLookup[m_PreviewSphereMesh] = 0;
		m_SceneData->TransformIDLookup[0] = glm::mat4(1.0f);

		m_SceneData->RenderQueue.push_back(0);

		// Lights
		glm::quat rot = glm::quat({ -0.7f, -glm::radians(45.0f), 0.0f });
		Light light(Light::LightType::Directional, 10.0f, { 1.0f, 1.0f, 1.0f });
		m_SceneData->Environment.DirectionalLights.push_back({ glm::vec3{ 0.0f }, rot, light });

		// Camera
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0, 0, 2 });
		CameraData cameraData(m_PreviewCamera, { 512, 512 }, transform);

		m_SceneData->CameraBuffer = UniformBuffer::Create(sizeof(CameraData));
		m_SceneData->CameraBuffer->Bind(0);
		m_SceneData->CameraBuffer->SetData(&cameraData, 0, sizeof(CameraData));

		// Uniform values
		m_SceneData->UniformValueBuffer = UniformBuffer::Create(2 << 14); // 16KB

		// Renderpasses
		m_GeometryPass = CreateRef<GeometryRenderPass>();
		m_LightingPass = CreateRef<LightingRenderPass>();
		m_TonemapPass = CreateRef<TonemappingRenderPass>();

		m_GeometryPass->Init(1, 1);
		m_LightingPass->Init(1, 1);
		m_TonemapPass->Init(1, 1);
#endif
	}

	void MaterialAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Material* material = static_cast<Material*>(asset);

		// Shader ID
		node["Shader"] << material->GetShader().GetID();

		// Material properties
		ryml::NodeRef propertiesNode = node["Properties"];
		propertiesNode |= ryml::MAP;

		const UnorderedMap<std::string, ShaderProperty>& properties = material->GetShader()->GetProperties();
		for (auto& kv : properties)
		{
			switch (kv.second.DataType)
			{
			case ShaderDataType::Float:			propertiesNode[ryml::to_csubstr(kv.first)] << material->GetFloat(kv.first); break;
			case ShaderDataType::Float2:		propertiesNode[ryml::to_csubstr(kv.first)] << material->GetFloat2(kv.first); break;
			case ShaderDataType::Float3:		propertiesNode[ryml::to_csubstr(kv.first)] << material->GetFloat3(kv.first); break;
			case ShaderDataType::Float4:		propertiesNode[ryml::to_csubstr(kv.first)] << material->GetFloat4(kv.first); break;
			case ShaderDataType::Mat3:			propertiesNode[ryml::to_csubstr(kv.first)] << material->GetMat3(kv.first); break;
			case ShaderDataType::Mat4:			propertiesNode[ryml::to_csubstr(kv.first)] << material->GetMat4(kv.first); break;
			case ShaderDataType::Int:			propertiesNode[ryml::to_csubstr(kv.first)] << material->GetInt(kv.first); break;
			case ShaderDataType::Sampler2D:		propertiesNode[ryml::to_csubstr(kv.first)] << material->GetTexture(kv.first).GetID(); break;
			case ShaderDataType::SamplerCube:	propertiesNode[ryml::to_csubstr(kv.first)] << material->GetTexture(kv.first).GetID(); break;
			default: break;
			}
		}
	}

	Asset<void> MaterialAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		Asset<Shader> shader;
		if (node.has_child("Shader"))
		{
			uint64_t assetID;
			node["Shader"] >> assetID;
			shader = Asset<Shader>(assetID);
		}

		if (shader)
		{
			Asset<Material> material = Material::Create(shader);

			const UnorderedMap<std::string, ShaderProperty>& properties = shader->GetProperties();
			if (node.has_child("Properties"))
			{
				ryml::NodeRef propertiesNode = node["Properties"];
				for (auto propertyNode : propertiesNode)
				{
					ryml::csubstr key = propertyNode.key();
					std::string propertyName(key.str, key.size());

					auto iter = properties.find(propertyName);
					if (iter != properties.end())
					{
						switch (iter->second.DataType)
						{
						case ShaderDataType::Float:			MH_CONDITIONAL_SET_VALUE(material->SetFloat, float);
						case ShaderDataType::Float2:		MH_CONDITIONAL_SET_VALUE(material->SetFloat2, glm::vec2);
						case ShaderDataType::Float3:		MH_CONDITIONAL_SET_VALUE(material->SetFloat3, glm::vec3);
						case ShaderDataType::Float4:		MH_CONDITIONAL_SET_VALUE(material->SetFloat4, glm::vec4);
						case ShaderDataType::Mat3:			MH_CONDITIONAL_SET_VALUE(material->SetMat3, glm::mat3);
						case ShaderDataType::Mat4:			MH_CONDITIONAL_SET_VALUE(material->SetMat4, glm::mat4);
						case ShaderDataType::Int:			MH_CONDITIONAL_SET_VALUE(material->SetInt, int);
						case ShaderDataType::Sampler2D:
						case ShaderDataType::SamplerCube:
						{
							uint64_t textureID;
							propertyNode >> textureID;
							if (textureID)
								material->SetTexture(propertyName, 0, Asset<Texture>(textureID));
							else
								material->SetTexture(propertyName, 0, GetDefaultTexture(iter->second));

							break;
						}
						default:
							break;
						}
					}
				}
			}
			return material;
		}

		return nullptr;
	}

	Asset<Texture> MaterialAssetImporter::GetDefaultTexture(const ShaderProperty& property)
	{
		ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(property.DefaultString));

		ryml::NodeRef rootNode = tree.rootref();

		if (rootNode.has_child("Value"))
		{
			ryml::NodeRef defaultNode = rootNode["Value"];

			std::string defaultString;
			switch (property.DataType)
			{
			case ShaderDataType::Sampler2D:
				defaultNode >> defaultString;

				if (defaultString == "White")
					return GL::GetTexture2DWhite();
				else if (defaultString == "Black")
					return GL::GetTexture2DBlack();
				else if (defaultString == "Bump")
					return GL::GetTexture2DBump();
				else
					MH_WARN("Could not find default Texture2D of type: {0}", defaultString);

				break;
			case ShaderDataType::SamplerCube:
				defaultNode >> defaultString;

				if (defaultString == "White")
					return GL::GetTextureCubeWhite();
				else if (defaultString == "Black")
					return GL::GetTextureCubeWhite();
				else
					MH_WARN("Could not find default TextureCube of type: {0}", defaultString);

				break;
			default:
				break;
			}

			return nullptr;
		}

		return nullptr;
	}

	void MaterialAssetImporter::SetupMaterialProperties(const UnorderedMap<std::string, ShaderProperty>& shaderProperties)
	{
		m_DefaultTextures.clear();

		m_MaterialProperties = shaderProperties;

		// Setup default material properties
		for (auto& kv : m_MaterialProperties)
		{
			std::string propertyName = kv.first;

			if (kv.second.DefaultString.empty())
				continue;

			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(kv.second.DefaultString));

			ryml::NodeRef rootNode = tree.rootref();

			if (rootNode.has_child("Value"))
			{
				ryml::NodeRef propertyNode = rootNode["Value"];
				std::string defaultString;
				switch (kv.second.DataType)
				{
				case ShaderDataType::Float:			MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat, float);
				case ShaderDataType::Float2:		MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat2, glm::vec2);
				case ShaderDataType::Float3:		MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat3, glm::vec3);
				case ShaderDataType::Float4:		MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat4, glm::vec4);
				case ShaderDataType::Mat3:			MH_CONDITIONAL_SET_VALUE(m_Material->SetMat3, glm::mat3);
				case ShaderDataType::Mat4:			MH_CONDITIONAL_SET_VALUE(m_Material->SetMat4, glm::mat4);
				case ShaderDataType::Int:			MH_CONDITIONAL_SET_VALUE(m_Material->SetInt, int);
				case ShaderDataType::Sampler2D:
					propertyNode >> defaultString;

					if (defaultString == "White")
						m_DefaultTextures[propertyName] = GL::GetTexture2DWhite();
					else if (defaultString == "Black")
						m_DefaultTextures[propertyName] = GL::GetTexture2DBlack();
					else if (defaultString == "Bump")
						m_DefaultTextures[propertyName] = GL::GetTexture2DBump();
					else
						MH_WARN("Could not find default Texture2D of type: {0}", defaultString);

					m_Material->SetTexture(propertyName, 0, m_DefaultTextures[propertyName]);

					break;
				case ShaderDataType::SamplerCube:
					propertyNode >> defaultString;

					if (defaultString == "White")
						m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();
					else if (defaultString == "Black")
						m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();
					else
						MH_WARN("Could not find default TextureCube of type: {0}", defaultString);

					m_Material->SetTexture(propertyName, 0, m_DefaultTextures[propertyName]);

					break;
				default:
					break;
				}
			}
		}

#ifndef MH_STANDALONE
		// Setup scene data for the material
		m_SceneData->ShaderIDLookup.clear();
		m_SceneData->MaterialIDLookup.clear();

		m_SceneData->ShaderRefLookup.clear();
		m_SceneData->MaterialRefLookup.clear();

		if (m_Material)
		{
			Asset<Shader> shader = m_Material->GetShader();

			m_SceneData->ShaderIDLookup[0] = shader;
			m_SceneData->ShaderRefLookup[shader] = 0;

			m_SceneData->MaterialIDLookup[0] = m_Material;
			m_SceneData->MaterialRefLookup[m_Material] = 0;
		}
#endif
	}
}

#undef MH_CONDITIONAL_COLOR
#undef MH_CONDITIONAL_SLIDER
#undef MH_CONDITIONAL_DRAG
#undef MH_CONDITIONAL_SET_VALUE