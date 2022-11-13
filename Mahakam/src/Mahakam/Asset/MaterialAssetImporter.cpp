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
		m_ImporterProps.Extension = ".material";
		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;


#ifndef MH_STANDALONE
		// Preview stuff
		m_PreviewSphereMesh = SubMesh::CreateUVSphere(20, 20);

		m_PreviewCamera = Camera(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.03f, 5.0f);

		// Setup scene data
		m_SceneData = CreateRef<SceneData>();
		m_SceneData->meshIDLookup[0] = m_PreviewSphereMesh;
		m_SceneData->meshRefLookup[m_PreviewSphereMesh] = 0;
		m_SceneData->transformIDLookup[0] = glm::mat4(1.0f);

		m_SceneData->renderQueue.push_back(0);

		// Lights
		glm::quat rot = glm::quat({ -0.7f, -glm::radians(45.0f), 0.0f });
		Light light(Light::LightType::Directional, 10.0f, { 1.0f, 1.0f, 1.0f });
		m_SceneData->environment.DirectionalLights.push_back({ glm::vec3{ 0.0f }, rot, light });

		// Camera
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { 0, 0, 2 });
		CameraData cameraData(m_PreviewCamera, { 512, 512 }, transform);

		m_SceneData->cameraBuffer = UniformBuffer::Create(sizeof(CameraData));
		m_SceneData->cameraBuffer->Bind(0);
		m_SceneData->cameraBuffer->SetData(&cameraData, 0, sizeof(CameraData));

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

#ifndef MH_STANDALONE
	void MaterialAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		// Load the preview skybox
		Asset<Material> skyboxMaterial = Asset<Material>("import/assets/materials/internal/PreviewSky.material.import");
		Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>("import/assets/textures/internal/previewirradiance.hdr.import");
		Asset<TextureCube> skyboxSpecular = Asset<TextureCube>("import/assets/textures/internal/previewspecular.hdr.import");

		m_SceneData->environment.SkyboxMaterial = skyboxMaterial;
		m_SceneData->environment.IrradianceMap = skyboxIrradiance;
		m_SceneData->environment.SpecularMap = skyboxSpecular;

		// Reset orbit angles
		m_OrbitEulerAngles = { 0.0f, 0.0f, 0.0f };

		if (node.valid() && node.has_child("Shader"))
		{
			uint64_t shaderID;
			node["Shader"] >> shaderID;
			Asset<Shader> shader(shaderID);
			m_ShaderImportPath = shader.GetImportPath();

			if (shader)
			{
				m_Material = Material::Create(shader);

				SetupMaterialProperties(shader->GetProperties());

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
							case ShaderDataType::Float:			MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat, float);
							case ShaderDataType::Float2:		MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat2, glm::vec2);
							case ShaderDataType::Float3:		MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat3, glm::vec3);
							case ShaderDataType::Float4:		MH_CONDITIONAL_SET_VALUE(m_Material->SetFloat4, glm::vec4);
							case ShaderDataType::Mat3:			MH_CONDITIONAL_SET_VALUE(m_Material->SetMat3, glm::mat3);
							case ShaderDataType::Mat4:			MH_CONDITIONAL_SET_VALUE(m_Material->SetMat4, glm::mat4);
							case ShaderDataType::Int:			MH_CONDITIONAL_SET_VALUE(m_Material->SetInt, int);
							case ShaderDataType::Sampler2D:
							case ShaderDataType::SamplerCube:
							{
								uint64_t textureID;
								propertyNode >> textureID;
								if (textureID)
									m_Material->SetTexture(propertyName, 0, Asset<Texture>(textureID));

								break;
							}
							default:
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			m_ShaderImportPath = "";
			m_Material = nullptr;

			SetupMaterialProperties({});
		}
	}

	void MaterialAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		if (GUI::DrawDragDropField("Shader", ".shader", m_ShaderImportPath))
		{
			if (std::filesystem::exists(m_ShaderImportPath))
			{
				Asset<Shader> shader = Asset<Shader>(m_ShaderImportPath);

				if (shader)
				{
					m_Material = Material::Create(shader);
					SetupMaterialProperties(shader->GetProperties());
				}
				else
				{
					SetupMaterialProperties({});
				}
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

			std::filesystem::path texturePath;
			Asset<Texture> texture;

			switch (propertyType)
			{
			case ShaderPropertyType::HDR: // Color edit is the same for HDR, just with some extra flags
				colorFlags |= ImGuiColorEditFlags_HDR;
			case ShaderPropertyType::Color: // Color editor
				switch (dataType)
				{
				case ShaderDataType::Float3:	MH_CONDITIONAL_COLOR(GUI::DrawColor3Edit, glm::vec3, SetFloat3, GetFloat3);
				case ShaderDataType::Float4:	MH_CONDITIONAL_COLOR(GUI::DrawColor4Edit, glm::vec4, SetFloat4, GetFloat4);
				default: break;
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
				default: break;
				}
				break;
			case ShaderPropertyType::Drag: // Drag editor
				switch (dataType)
				{
				case ShaderDataType::Float:		MH_CONDITIONAL_DRAG(GUI::DrawFloatDrag, float, SetFloat, GetFloat);
				case ShaderDataType::Float2:	MH_CONDITIONAL_DRAG(GUI::DrawFloat2Drag, glm::vec2, SetFloat2, GetFloat2);
				case ShaderDataType::Float3:	MH_CONDITIONAL_DRAG(GUI::DrawFloat3Drag, glm::vec3, SetFloat3, GetFloat3);
				case ShaderDataType::Float4:	MH_CONDITIONAL_DRAG(GUI::DrawFloat4Drag, glm::vec4, SetFloat4, GetFloat4);
				default: break;
				}
				break;
			case ShaderPropertyType::Texture: // Textures and normals are handled the same, for now
			case ShaderPropertyType::Normal:
				texture = m_Material->GetTexture(propertyName);
				if (texture)
					texturePath = texture.GetImportPath();
				if (GUI::DrawDragDropField(propertyName, ".texture", texturePath))
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
			default:
				break;
			}
		}

		// Render a material sphere preview
		if (m_Material)
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			float maxSize = glm::min(size.x, size.y);
			ImVec2 viewportSize = { size.x, maxSize };

			// Update renderpasses
			if (viewportSize.x != m_ViewportSize.x && viewportSize.y != m_ViewportSize.y)
			{
				m_ViewportSize = { (uint32_t)viewportSize.x, (uint32_t)viewportSize.y };
				m_GeometryPass->OnWindowResize(m_ViewportSize.x, m_ViewportSize.y);
				m_LightingPass->OnWindowResize(m_ViewportSize.x, m_ViewportSize.y);
				m_TonemapPass->OnWindowResize(m_ViewportSize.x, m_ViewportSize.y);
			}

			// Setup camera
			m_PreviewCamera.SetRatio(viewportSize.x / viewportSize.y);
			m_PreviewCamera.RecalculateProjectionMatrix();
			glm::quat rot = glm::quat(m_OrbitEulerAngles);
			glm::mat4 transform = glm::toMat4(rot) * glm::translate(glm::mat4(1.0f), { 0, 0, 1.5f });

			CameraData cameraData(m_PreviewCamera, { 512, 512 }, transform);

			m_SceneData->cameraBuffer->Bind(0);
			m_SceneData->cameraBuffer->SetData(&cameraData, 0, sizeof(CameraData));

			// Setup uniform values
			m_SceneData->UniformValueBuffer->Bind(3);

			// Render the material
			Asset<FrameBuffer> nullBuffer = nullptr;
			m_GeometryPass->Render(m_SceneData.get(), nullBuffer);
			m_LightingPass->Render(m_SceneData.get(), m_GeometryPass->GetFrameBuffer());
			m_TonemapPass->Render(m_SceneData.get(), m_LightingPass->GetFrameBuffer());

			ImGui::BeginChild("Material Preview", viewportSize);
			bool focused = ImGui::IsWindowFocused();

			// Calculate mouse delta
			auto [mouseX, mouseY] = Input::GetMousePos();
			float deltaX = mouseX - m_MousePos.x;
			float deltaY = mouseY - m_MousePos.y;
			m_MousePos.x = mouseX;
			m_MousePos.y = mouseY;

			if (focused && Input::IsMouseButtonPressed(MouseButton::LEFT))
			{
				m_OrbitEulerAngles.y -= glm::radians(deltaX * m_DragSpeed);
				m_OrbitEulerAngles.x = glm::clamp(m_OrbitEulerAngles.x - glm::radians(deltaY * m_DragSpeed), -1.5707f, 1.5707f);
			}

			ImGui::Image((void*)(uintptr_t)m_TonemapPass->GetFrameBuffer()->GetColorTexture(0)->GetRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

			ImGui::EndChild();
		}
	}

	void MaterialAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		m_Material.Save(filepath, importPath);

		//AssetDatabase::ReloadAsset(m_Material.GetID());

		// Reset so we don't keep resources in memory
		m_ShaderImportPath = "";
		m_Material = nullptr;
		SetupMaterialProperties({});
	}
#endif

	void MaterialAssetImporter::Serialize(ryml::NodeRef& node, Asset<void> asset)
	{
		Asset<Material> material(asset);

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
					MH_CORE_WARN("Could not find default Texture2D of type: {0}", defaultString);

				break;
			case ShaderDataType::SamplerCube:
				defaultNode >> defaultString;

				if (defaultString == "White")
					return GL::GetTextureCubeWhite();
				else if (defaultString == "Black")
					return GL::GetTextureCubeWhite();
				else
					MH_CORE_WARN("Could not find default TextureCube of type: {0}", defaultString);

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
						MH_CORE_WARN("Could not find default Texture2D of type: {0}", defaultString);

					m_Material->SetTexture(propertyName, 0, m_DefaultTextures[propertyName]);

					break;
				case ShaderDataType::SamplerCube:
					propertyNode >> defaultString;

					if (defaultString == "White")
						m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();
					else if (defaultString == "Black")
						m_DefaultTextures[propertyName] = GL::GetTextureCubeWhite();
					else
						MH_CORE_WARN("Could not find default TextureCube of type: {0}", defaultString);

					m_Material->SetTexture(propertyName, 0, m_DefaultTextures[propertyName]);

					break;
				default:
					break;
				}
			}
		}

		// Setup scene data for the material
		m_SceneData->shaderIDLookup.clear();
		m_SceneData->materialIDLookup.clear();

		m_SceneData->shaderRefLookup.clear();
		m_SceneData->materialRefLookup.clear();

		if (m_Material)
		{
			Asset<Shader> shader = m_Material->GetShader();

			m_SceneData->shaderIDLookup[0] = shader;
			m_SceneData->shaderRefLookup[shader] = 0;

			m_SceneData->materialIDLookup[0] = m_Material;
			m_SceneData->materialRefLookup[m_Material] = 0;
		}
	}
}

#undef MH_CONDITIONAL_COLOR
#undef MH_CONDITIONAL_SLIDER
#undef MH_CONDITIONAL_DRAG
#undef MH_CONDITIONAL_SET_VALUE