#include "ebpch.h"
#include "ImportWizardPanel.h"

namespace Mahakam::Editor
{
	void ImportWizardPanel::OnImGuiRender()
	{
		Ref<AssetImporter> importer;
		if (m_Open && (importer = m_Importer.lock()))
		{
			ImGui::Begin("Import Asset", &m_Open);

			importer->OnWizardRender(m_FilePath);

			ImGui::Separator();

			if (!importer->GetImporterProps().NoFilepath)
			{
				GUI::DrawDragDropTarget("File path", m_Extension, m_FilePath);

				std::string importString = m_ImportPath.string();
				char importBuffer[256]{ 0 };
				strncpy(importBuffer, importString.c_str(), importString.size() - 7);
				if (ImGui::InputText("Import path", importBuffer, 256))
				{
					m_ImportPath = std::string(importBuffer) + ".import";
				}
			}

			if (ImGui::Button("Import"))
			{
				Asset<void> asset(m_ImportPath);

				importer->OnWizardImport(asset, m_FilePath, m_ImportPath);

				m_Open = false;
			}

			ImGui::End();
		}
	}

	void ImportWizardPanel::ImportAsset(const std::filesystem::path& filepath, const std::string& extension, const std::filesystem::path& importPath)
	{
		ImportWizardPanel* window = (ImportWizardPanel*)EditorWindowRegistry::OpenWindow("Import Wizard");

		window->m_Open = true;

		window->m_Extension = extension;
		window->m_FilePath = filepath;
		window->m_ImportPath = importPath;

		YAML::Node data;
		if (std::filesystem::exists(importPath))
		{
			try
			{
				data = YAML::LoadFile(importPath.string());
			}
			catch (YAML::ParserException e)
			{
				MH_CORE_WARN("Weird yaml file found in {0}: {1}", importPath.string(), e.msg);
			}
		}

		window->m_Importer = AssetDatabase::GetAssetImporter(extension);
		if (Ref<AssetImporter> importer = window->m_Importer.lock())
			importer->OnWizardOpen(data);
	}
}