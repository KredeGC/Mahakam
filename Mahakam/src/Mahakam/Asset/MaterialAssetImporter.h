#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Material.h"

namespace Mahakam
{
	class MaterialAssetImporter : public AssetImporter
	{
	private:
		robin_hood::unordered_map<std::string, Asset<Texture>> m_Textures;

		robin_hood::unordered_map<std::string, glm::mat3> m_Mat3s;
		robin_hood::unordered_map<std::string, glm::mat4> m_Mat4s;

		robin_hood::unordered_map<std::string, int32_t> m_Ints;

		robin_hood::unordered_map<std::string, float> m_Floats;
		robin_hood::unordered_map<std::string, glm::vec2> m_Float2s;
		robin_hood::unordered_map<std::string, glm::vec3> m_Float3s;
		robin_hood::unordered_map<std::string, glm::vec4> m_Float4s;

	public:
		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender() override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;
	};
}