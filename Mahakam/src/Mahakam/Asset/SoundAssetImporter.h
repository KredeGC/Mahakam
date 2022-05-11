#pragma once

#include "AssetImporter.h"

#include "Mahakam/Audio/SoundProps.h"

namespace Mahakam
{
	class SoundAssetImporter : public AssetImporter
	{
	private:
		SoundProps m_Props;

	public:
		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender() override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;
	};
}