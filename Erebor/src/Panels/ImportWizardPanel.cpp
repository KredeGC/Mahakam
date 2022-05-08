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

			ImGui::TextWrapped("Asset path: %s", m_FilePath.string().c_str());
			ImGui::TextWrapped("Import path: %s", m_ImportPath.string().c_str());

			m_Importer->OnWizardRender();

			if (ImGui::Button("Import"))
			{
				Ref<void> asset = m_Importer->OnWizardImport(m_FilePath);

				AssetDatabase::SaveAsset(asset, m_FilePath.extension().string(), m_ImportPath);

				m_Open = false;
				m_Importer = nullptr;
			}

			ImGui::End();
		}
	}

	void ImportWizardPanel::ImportAsset(const std::filesystem::path& filepath, const std::filesystem::path& importPath)
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

		m_Importer = AssetDatabase::GetAssetImporter(filepath.extension().string());
		if (m_Importer)
			m_Importer->OnWizardOpen(data);
	}
}