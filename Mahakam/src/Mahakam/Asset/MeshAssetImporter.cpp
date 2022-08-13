#include "Mahakam/mhpch.h"
#include "MeshAssetImporter.h"

namespace Mahakam
{
	MeshAssetImporter::MeshAssetImporter()
	{
		m_ImporterProps.Extension = ".mesh";
	}

#ifndef MH_STANDALONE
	void MeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) { }

	void MeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath) { }

	void MeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) { }
#endif

	void MeshAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{

	}

	Asset<void> MeshAssetImporter::Deserialize(YAML::Node& node)
	{
		return nullptr;
	}
}