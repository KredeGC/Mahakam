#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/Camera.h"

namespace Mahakam
{
	// TODO:
	// Make a ShaderLibrary that maintains a reference to all loaded shaders

	class RenderPass;
	struct SceneData;

	class MaterialAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

		Asset<Material> m_Material;

		std::filesystem::path m_ShaderFilepath;

		UnorderedMap<std::string, ShaderProperty> m_MaterialProperties;
		
		UnorderedMap<std::string, Asset<Texture>> m_DefaultTextures;

		// Preview
		Asset<Mesh> m_PreviewSphereMesh;
		Camera m_PreviewCamera;
		//Asset<FrameBuffer> m_FrameBuffer;
		//Asset<UniformBuffer> m_CameraBuffer;
		Ref<SceneData> m_SceneData;
		glm::ivec2 m_ViewportSize;
		float m_Rotation = 0.0f;

		Ref<RenderPass> m_GeometryPass;
		Ref<RenderPass> m_LightingPass;
		Ref<RenderPass> m_TonemapPass;

	public:
		MaterialAssetImporter();

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;

	private:
		Asset<Texture> GetDefaultTexture(const ShaderProperty& property);

		void SetupMaterialProperties(const UnorderedMap<std::string, ShaderProperty>& shaderProperties, const std::filesystem::path& filepath);
	};
}