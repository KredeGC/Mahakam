#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Material.h"

namespace Mahakam
{
	// TODO:
	// Make a ShaderLibrary that maintains a reference to all loaded shaders

	class MaterialAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

		Asset<Material> m_Material;

		std::filesystem::path m_ShaderFilepath;

		UnorderedMap<std::string, ShaderProperty> m_MaterialProperties;
		
		UnorderedMap<std::string, Asset<Texture>> m_DefaultTextures;
		
		UnorderedMap<std::string, Asset<Texture>> m_Textures;
		
		UnorderedMap<std::string, glm::mat3> m_Mat3s;
		UnorderedMap<std::string, glm::mat4> m_Mat4s;
		
		UnorderedMap<std::string, int32_t> m_Ints;
		
		UnorderedMap<std::string, float> m_Floats;
		UnorderedMap<std::string, glm::vec2> m_Float2s;
		UnorderedMap<std::string, glm::vec3> m_Float3s;
		UnorderedMap<std::string, glm::vec4> m_Float4s;

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

		void SetupMaterialProperties(const std::unordered_map<std::string, ShaderProperty>& shaderProperties, const std::filesystem::path& filepath);
	};
}