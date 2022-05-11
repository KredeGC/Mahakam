#include "mhpch.h"
#include "MaterialAssetImporter.h"

#include <imgui.h>

namespace Mahakam
{
	void MaterialAssetImporter::OnWizardOpen(YAML::Node& node)
	{
		
	}

	void MaterialAssetImporter::OnWizardRender()
	{
		
	}

	void MaterialAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		
	}

	void MaterialAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		
	}

	Asset<void> MaterialAssetImporter::Deserialize(YAML::Node& node)
	{
		return nullptr;
	}
}