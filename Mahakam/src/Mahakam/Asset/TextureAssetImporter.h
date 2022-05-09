#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	class TextureAssetImporter : public AssetImporter
	{
	private:
		TextureProps m_Props2D;
		CubeTextureProps m_PropsCube;

		int m_TextureType = 0;

	public:
		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender() override;
		virtual Asset<void> OnWizardImport(const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;
	};
}