#pragma once

#include <Mahakam.h>

namespace Mahakam::Editor
{
	class ImportWizardPanel : public EditorWindow
	{
	private:
		static bool m_Open;

		static std::filesystem::path m_FilePath;
		static std::filesystem::path m_ImportPath;

		static WeakRef<AssetImporter> m_Importer;

	public:
		virtual void OnImGuiRender() override;

		static void ImportAsset(const std::filesystem::path& filepath, const std::string& extension, const std::filesystem::path& importPath);
	};
}