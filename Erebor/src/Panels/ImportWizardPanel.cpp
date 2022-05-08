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

			ImGui::TextWrapped("Base file path: %s", m_FilePath.string().c_str());
			std::string pathString = m_ImportPath.string();
			char pathBuffer[256]{ 0 };
			strncpy(pathBuffer, pathString.c_str(), pathString.size() - 5);
			if (ImGui::InputText("Import path", pathBuffer, 256))
			{
				m_ImportPath = std::string(pathBuffer) + ".yaml";
			}

			if (ImGui::Button("Import"))
			{
				Ref<void> asset = m_Importer->OnWizardImport(m_FilePath);

				AssetDatabase::SaveAsset(asset, m_FilePath, m_ImportPath);

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