#pragma once

#include "AssetImporter.h"

namespace Mahakam
{
	class ShaderAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

	public:
		ShaderAssetImporter();

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

#ifndef MH_STANDALONE
		virtual void OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;
#endif

		virtual void Serialize(ryml::NodeRef& node, void* asset) override;
		virtual Asset<void> Deserialize(ryml::NodeRef& node) override;
	};
}