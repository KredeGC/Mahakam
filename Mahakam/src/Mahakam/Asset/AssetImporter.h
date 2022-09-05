#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include <filesystem>
#include <string>

#include <yaml-cpp/yaml.h>

namespace Mahakam
{
	template<typename T>
	class Asset;

	extern template class Asset<void>;

	class AssetImporter
	{
	public:
		struct ImporterProps
		{
			bool CreateMenu = false;
			bool NoFilepath = false;
			bool NoWizard = false;
			std::string Extension;
		};

		virtual ~AssetImporter() = default;

		virtual const ImporterProps& GetImporterProps() const = 0;

#ifndef MH_STANDALONE
		virtual void OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& rootNode) = 0;
		virtual void OnWizardRender(const std::filesystem::path& filepath) = 0;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) = 0;
#endif

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) = 0;
		virtual Asset<void> Deserialize(YAML::Node& rootNode) = 0;
	};
}