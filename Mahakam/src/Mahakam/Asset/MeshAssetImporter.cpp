#include "mhpch.h"
#include "MeshAssetImporter.h"

namespace Mahakam
{
	MeshAssetImporter::MeshAssetImporter()
	{
		m_ImporterProps.Extension = ".mesh";
	}

	void MeshAssetImporter::OnWizardOpen(YAML::Node& node)
	{

	}

	void MeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{

	}

	void MeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{

	}

	void MeshAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{

	}

	Asset<void> MeshAssetImporter::Deserialize(YAML::Node& node)
	{
		return nullptr;
	}
}