#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include <filesystem>
#include <string>

#include <ryml/rapidyaml-0.4.1.hpp>

namespace Mahakam
{
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
		virtual void OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& rootNode) = 0;
		virtual void OnWizardRender(const std::filesystem::path& filepath) = 0;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) = 0;
#endif

		virtual void Serialize(ryml::NodeRef& node, Asset<void> asset) = 0;
		virtual Asset<void> Deserialize(ryml::NodeRef& node) = 0;
	};
}