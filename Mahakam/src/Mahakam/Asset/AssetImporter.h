#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Asset/Asset.h"

#include <filesystem>
#include <string>

#include <yaml-cpp/yaml.h>

namespace Mahakam
{
	class AssetImporter
	{
	public:
		virtual void OnWizardOpen(YAML::Node& node) = 0;
		virtual void OnWizardRender() = 0;
		//virtual Asset<void> OnWizardImport(const std::filesystem::path& filepath, const std::filesystem::path& importPath) = 0;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) = 0;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) = 0;
		virtual Asset<void> Deserialize(YAML::Node& node) = 0;
	};
}