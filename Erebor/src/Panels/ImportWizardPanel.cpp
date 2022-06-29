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
				GUI::DrawDragDropField("File path", m_Extension, m_FilePath);

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
		if (Ref<AssetImporter> importer = AssetDatabase::GetAssetImporter(extension))
		{
			if (importer->GetImporterProps().NoWizard)
			{
				Asset<void> asset(importPath);

				importer->OnWizardImport(asset, filepath, importPath);
			}
			else
			{
				ImportWizardPanel* window = (ImportWizardPanel*)EditorWindowRegistry::OpenWindow("Import Wizard");

				window->m_Open = true;
				window->m_Importer = importer;

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

				importer->OnWizardOpen(data);
			}
		}
	}
}