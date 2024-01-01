#pragma once

#include "ResourceImporter.h"

#include "Mahakam/Renderer/Camera.h"

#include "Mahakam/Renderer/ShaderProps.h"

namespace Mahakam
{
	class Camera;
	class FrameBuffer;
	class Material;
	class RenderPass;
	class Shader;
	class SubMesh;
	class Texture;

	struct SceneData;

	class MaterialResourceImporter : public ResourceImporter
	{
	private:
		const float m_DragSpeed = 0.3f;

		Asset<Material> m_Material;

		std::filesystem::path m_ShaderImportPath;

		UnorderedMap<std::string, ShaderProperty> m_MaterialProperties;

		UnorderedMap<std::string, Asset<Texture>> m_DefaultTextures;

		// Preview
		Ref<SubMesh> m_PreviewSphereMesh;
		Camera m_PreviewCamera;
		Scope<SceneData> m_SceneData;
		glm::ivec2 m_ViewportSize{ 0 };
		glm::vec2 m_MousePos{ 0.0f };
		glm::vec3 m_OrbitEulerAngles{ 0.0f };

		Scope<RenderPass> m_GeometryPass;
		Scope<RenderPass> m_LightingPass;
		Scope<RenderPass> m_TonemapPass;

	public:
		MaterialResourceImporter();

		virtual void OnResourceOpen(const std::filesystem::path& filepath) override {} // Unused
		virtual void OnImportOpen(ryml::NodeRef& node) override;
		virtual void OnRender() override;
		virtual void OnImport(ryml::NodeRef& node) override;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) override;

	private:
		Asset<Texture> GetDefaultTexture(const ShaderProperty& property);

		void SetupMaterialProperties(const UnorderedMap<std::string, ShaderProperty>& shaderProperties);
	};
}