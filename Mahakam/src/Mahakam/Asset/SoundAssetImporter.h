#pragma once

#include "AssetImporter.h"

#include "Mahakam/Audio/SoundProps.h"

namespace Mahakam
{
	class SoundAssetImporter : public AssetImporter
	{
	private:
		SoundProps soundProps;

	public:
		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender() override;
		virtual void OnWizardImport(Ref<void>& asset, const std::filesystem::path& filepath) override;

		virtual void Serialize(YAML::Emitter& emitter, Ref<void> asset) override;
		virtual Ref<void> Deserialize(YAML::Node& node) override;
	};
}