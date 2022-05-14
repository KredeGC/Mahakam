#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Material.h"

namespace Mahakam
{
	enum class MaterialPropertyType
	{
		Color,
		HDR,
		Vector,
		Range,
		Drag,
		Default
	};

	struct MaterialProperty
	{
		MaterialPropertyType PropertyType;
		ShaderDataType DataType;
		float Min = -std::numeric_limits<float>::infinity();
		float Max = std::numeric_limits<float>::infinity();
	};

	class MaterialAssetImporter : public AssetImporter
	{
	private:
		std::string m_ShaderFilepath;

		ImporterProps m_ImporterProps;

		std::unordered_map<std::string, MaterialProperty> m_MaterialProperties;

		std::unordered_map<std::string, Asset<Texture>> m_Textures;
		
		std::unordered_map<std::string, glm::mat3> m_Mat3s;
		std::unordered_map<std::string, glm::mat4> m_Mat4s;
		
		std::unordered_map<std::string, int32_t> m_Ints;
		
		std::unordered_map<std::string, float> m_Floats;
		std::unordered_map<std::string, glm::vec2> m_Float2s;
		std::unordered_map<std::string, glm::vec3> m_Float3s;
		std::unordered_map<std::string, glm::vec4> m_Float4s;

	public:
		MaterialAssetImporter();

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;

	private:
		void SetupMaterialProperties(const std::unordered_map<std::string, ShaderElement>& shaderProperties, const std::filesystem::path& filepath);
	};
}