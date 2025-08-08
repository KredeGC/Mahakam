#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include <filesystem>
#include <functional>
#include <string>

#include <ryml/rapidyaml-0.4.1.hpp>

namespace Mahakam
{
	class ResourceImporter
	{
	public:
		struct ImporterProps
		{
			bool CreateMenu = false;
			bool NoFilepath = false;
			bool NoWizard = false;
			std::string Name;
			std::string Extension;
			std::string Type;

			ImporterProps(const std::string& name, const std::string& extension, const std::string& type) :
				CreateMenu(false),
				NoFilepath(false),
				NoWizard(false),
				Name(name),
				Extension(extension),
				Type(type) {}
		};

		ResourceImporter(const std::string& name, const std::string& extension, const std::string& type) :
			m_ImporterProps(name, extension, type) {}

		virtual ~ResourceImporter() = default;

		const ImporterProps& GetImporterProps() const { return m_ImporterProps; }

		// Called when importing a raw resource (by opening the raw file)
		virtual void OnResourceOpen(const std::filesystem::path& filepath) = 0;

		// Called when importing an existing resource (by opening the import file)
		virtual void OnImportOpen(ryml::NodeRef& rootNode) = 0;

		// Called when rendering the import wizard
		virtual void OnRender() = 0;

		// Called when importing an asset
		// Should be stateless
		virtual void OnImport(ryml::NodeRef& node) = 0;

		//virtual bool OnIconRender(Asset<void> asset) { return false; }

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) = 0;

	protected:
		ImporterProps m_ImporterProps;
	};
}