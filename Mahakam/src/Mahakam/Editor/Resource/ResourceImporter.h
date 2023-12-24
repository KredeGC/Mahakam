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

			ImporterProps(const std::string& name, const std::string& extension) :
				CreateMenu(false),
				NoFilepath(false),
				NoWizard(false),
				Name(name),
				Extension(extension) {}
		};

		ResourceImporter(const std::string& name, const std::string& extension) :
			m_ImporterProps(name, extension) {}

		virtual ~ResourceImporter() = default;

		const ImporterProps& GetImporterProps() const { return m_ImporterProps; }

		virtual void OnResourceOpen(const std::filesystem::path& filepath) = 0;
		virtual void OnImportOpen(ryml::NodeRef& rootNode) = 0;
		virtual void OnRender() = 0;
        //virtual bool OnIconRender(Asset<void> asset) { return false; }
		virtual void OnImport(ryml::NodeRef& node) = 0;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) = 0;

	protected:
		ImporterProps m_ImporterProps;
	};
}