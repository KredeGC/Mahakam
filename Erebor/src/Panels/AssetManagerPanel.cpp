#include "ebpch.h"
#include "AssetManagerPanel.h"

namespace Mahakam::Editor
{
	AssetManagerPanel::AssetManagerPanel()
	{

	}

	void AssetManagerPanel::OnImGuiRender()
	{
		if (ImGui::Begin("Asset Manager", &m_Open))
		{
			// Search field, with dragdrop
			const auto& importers = AssetDatabase::GetAssetImporters();
			std::vector<std::string> extensions;
			for (auto& importer : importers)
				extensions.push_back(importer.first);

			std::filesystem::path search = m_SearchString;
			if (GUI::DrawDragDropField("Search", extensions, search))
				m_SearchString = search.string();

			// TODO: Sort by references

			// List all assets within search field
			if (ImGui::BeginTable("Asset Table", 1, ImGuiTableFlags_Borders))
			{
				auto& assets = ResourceRegistry::GetImports();
				for (auto& asset : assets)
				{
					std::string assetIDString = std::to_string(asset.first);

					bool idMatch = assetIDString.find(m_SearchString) != std::string::npos;
					bool typeMatch = asset.second.Type.find(m_SearchString) != std::string::npos;
					bool pathMatch = asset.second.Filepath.string().find(m_SearchString) != std::string::npos;

					if (idMatch || typeMatch || pathMatch)
					{
						ImGui::TableNextColumn();

						ImGui::TextWrapped("ID: %s", assetIDString.c_str());
						ImGui::TextWrapped("Type: %s", asset.second.Type.c_str());
						ImGui::TextWrapped("Import path: %s", asset.second.Filepath.string().c_str());

						size_t assetCount = AssetDatabase::GetAssetReferences(asset.first);
						std::string assetCountString = std::to_string(assetCount);
						ImGui::TextWrapped("References: %s", assetCountString.c_str());
					}
				}

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}
}