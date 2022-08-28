#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/Camera.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/Shader.h"

namespace Mahakam
{
	class RenderPass;
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

		// Preview
		Asset<Mesh> m_PreviewSphereMesh;
		Camera m_PreviewCamera;
		Ref<SceneData> m_SceneData;
		glm::ivec2 m_ViewportSize{ 0 };
		glm::vec2 m_MousePos{ 0.0f };
		glm::vec3 m_OrbitEulerAngles{ 0.0f };

		Ref<RenderPass> m_GeometryPass;
		Ref<RenderPass> m_LightingPass;
		Ref<RenderPass> m_TonemapPass;

	public:
		MaterialAssetImporter();

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

#ifndef MH_STANDALONE
		virtual void OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;
#endif

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;

	private:
		Asset<Texture> GetDefaultTexture(const ShaderProperty& property);

		void SetupMaterialProperties(const UnorderedMap<std::string, ShaderProperty>& shaderProperties);
	};
}