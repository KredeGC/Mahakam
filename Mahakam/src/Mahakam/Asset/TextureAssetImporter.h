#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	class TextureAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

		TextureProps m_Props2D;
		CubeTextureProps m_PropsCube;

		int m_TextureType = 0;

	public:
		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender() override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;
	};
}