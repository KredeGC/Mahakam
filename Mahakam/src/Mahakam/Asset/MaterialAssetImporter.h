#pragma once

#include "AssetImporter.h"

#include "Mahakam/Renderer/Camera.h"

#include "Mahakam/Renderer/ShaderDataTypes.h"

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

	class MaterialAssetImporter : public AssetImporter
	{
	private:
		const float m_DragSpeed = 0.3f;

		ImporterProps m_ImporterProps;

		Asset<Material> m_Material;

		std::filesystem::path m_ShaderImportPath;

		UnorderedMap<std::string, ShaderProperty> m_MaterialProperties;
		
		UnorderedMap<std::string, Asset<Texture>> m_DefaultTextures;

#ifndef MH_STANDALONE
		// Preview
		Ref<SubMesh> m_PreviewSphereMesh;
		Camera m_PreviewCamera;
		Ref<SceneData> m_SceneData;
		glm::ivec2 m_ViewportSize{ 0 };
		glm::vec2 m_MousePos{ 0.0f };
		glm::vec3 m_OrbitEulerAngles{ 0.0f };

		Ref<RenderPass> m_GeometryPass;
		Ref<RenderPass> m_LightingPass;
		Ref<RenderPass> m_TonemapPass;
#endif

	public:
		MaterialAssetImporter();

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

#ifndef MH_STANDALONE
		virtual void OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node) override {}
		virtual void OnWizardRender(const std::filesystem::path& filepath) override {}
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override {}
#endif

		virtual void Serialize(ryml::NodeRef& node, void* asset) override;
		virtual Asset<void> Deserialize(ryml::NodeRef& node) override;

	private:
		Asset<Texture> GetDefaultTexture(const ShaderProperty& property);

		void SetupMaterialProperties(const UnorderedMap<std::string, ShaderProperty>& shaderProperties);
	};
}