#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class ImportWizardPanel : public EditorWindow
	{
	private:
		bool m_Open = true;

		std::filesystem::path m_ImportPath;
		AssetDatabase::AssetID m_AssetID;

		WeakRef<ResourceImporter> m_Importer;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

		static void ResourceOpen(const std::filesystem::path& filepath, const std::string& extension);
		static void ImportOpen(const std::filesystem::path& importPath, const std::string& extension);
	};
}