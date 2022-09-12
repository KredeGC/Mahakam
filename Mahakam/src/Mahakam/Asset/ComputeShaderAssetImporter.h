#pragma once

#include "AssetImporter.h"

namespace Mahakam
{
	class ComputeShaderAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

	public:
		ComputeShaderAssetImporter();

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

#ifndef MH_STANDALONE
		virtual void OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;
#endif

		virtual void Serialize(YAML::Emitter& emitter, Ref<void> asset) override;
		virtual Ref<void> Deserialize(YAML::Node& node) override;
	};
}