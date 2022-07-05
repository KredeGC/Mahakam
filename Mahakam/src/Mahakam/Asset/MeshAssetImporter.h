#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Mesh.h"

namespace Mahakam
{
	class MeshAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

	public:
		MeshAssetImporter();

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

		virtual void OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;
	};
}