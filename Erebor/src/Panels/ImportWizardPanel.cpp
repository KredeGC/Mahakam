#include "ebpch.h"
#include "ImportWizardPanel.h"

namespace Mahakam::Editor
{
	bool ImportWizardPanel::m_Open;

	std::filesystem::path ImportWizardPanel::m_FilePath;
	std::filesystem::path ImportWizardPanel::m_ImportPath;

	Ref<AssetImporter> ImportWizardPanel::m_Importer;

	void ImportWizardPanel::OnImGuiRender()
	{
		if (m_Open && m_Importer)
		{
			ImGui::Begin("Import Asset", &m_Open);

			m_Importer->OnWizardRender();

			ImGui::Separator();

			std::string filepathString = m_FilePath.string();
			char filepathBuffer[256]{ 0 };
			strncpy(filepathBuffer, filepathString.c_str(), filepathString.size());
			if (ImGui::InputText("File path", filepathBuffer, 256))
				m_FilePath = std::string(filepathBuffer);

			std::string importString = m_ImportPath.string();
			char importBuffer[256]{ 0 };
			strncpy(importBuffer, importString.c_str(), importString.size() - 5);
			if (ImGui::InputText("Import path", importBuffer, 256))
			{
				m_ImportPath = std::string(importBuffer) + ".yaml";
			}

			if (ImGui::Button("Import"))
			{
				Asset<void> asset(m_ImportPath);

				m_Importer->OnWizardImport(asset, m_FilePath, m_ImportPath);

				m_Open = false;
				m_Importer = nullptr;
			}

			ImGui::End();
		}
	}

	void ImportWizardPanel::ImportAsset(const std::filesystem::path& filepath, const std::string& extension, const std::filesystem::path& importPath)
	{
		m_Open = true;

		m_FilePath = filepath;
		m_ImportPath = importPath;

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

		m_Importer = AssetDatabase::GetAssetImporter(extension);
		if (m_Importer)
			m_Importer->OnWizardOpen(data);
	}
}