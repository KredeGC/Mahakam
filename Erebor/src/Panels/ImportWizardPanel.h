#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class ImportWizardPanel : public EditorWindow
	{
	private:
		bool m_Open = true;

		AssetDatabase::Extension m_Extension;
		std::filesystem::path m_FilePath;
		std::filesystem::path m_ImportPath;

		WeakRef<AssetImporter> m_Importer;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

		static void ImportAsset(const std::filesystem::path& filepath, const AssetDatabase::Extension& extension, const std::filesystem::path& importPath);
	};
}