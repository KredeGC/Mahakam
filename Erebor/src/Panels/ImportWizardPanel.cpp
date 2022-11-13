#include "ebpch.h"
#include "ImportWizardPanel.h"

namespace Mahakam::Editor
{
	void ImportWizardPanel::OnImGuiRender()
	{
		Ref<AssetImporter> importer;
		if (m_Open && (importer = m_Importer.lock()))
		{
			if (ImGui::Begin("Import Asset", &m_Open))
			{
				importer->OnWizardRender(m_FilePath);

				ImGui::Separator();

				// Draw filepath input
				if (!importer->GetImporterProps().NoFilepath)
				{
					GUI::DrawDragDropField("File path", m_Extension, m_FilePath);
				}

				// Draw filename input without path/**.extension.import
				std::string nameString = m_ImportPath.stem().stem().string();
				char nameBuffer[GUI::MAX_STR_LEN]{ 0 };
				strncpy(nameBuffer, nameString.c_str(), nameString.size());
				if (ImGui::InputText("Import filename", nameBuffer, GUI::MAX_STR_LEN))
				{
					m_ImportPath = m_ImportPath.parent_path() / (std::string(nameBuffer) + importer->GetImporterProps().Extension + ".import");
				}

				// Draw import path input without the .extension.import
				std::string importString = (m_ImportPath.parent_path() / m_ImportPath.stem().stem()).string();
				char importBuffer[GUI::MAX_STR_LEN]{ 0 };
				strncpy(importBuffer, importString.c_str(), importString.size());
				if (ImGui::InputText("Import path", importBuffer, GUI::MAX_STR_LEN))
				{
					m_ImportPath = std::string(importBuffer) + importer->GetImporterProps().Extension + ".import";
				}

				// Draw import button
				if (ImGui::Button("Import"))
				{
					Asset<void> asset(m_ImportPath);

					importer->OnWizardImport(asset, m_FilePath, m_ImportPath);

					m_Open = false;
				}
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

				if (FileUtility::Exists(importPath))
				{
					std::vector<char> buffer;

					if (!FileUtility::ReadFile(importPath, buffer))
						return;

					try
					{
						ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(buffer));

						ryml::NodeRef root = tree.rootref();

						importer->OnWizardOpen(filepath, root);

						return;
					}
					catch (std::runtime_error const& e)
					{
						MH_CORE_WARN("Asset Import Wizard experienced an error when reading {0}: {1}", filepath.string(), e.what());
					}
				}
				else
				{
					ryml::NodeRef root;

					importer->OnWizardOpen(filepath, root);
				}
			}
		}
	}
}