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
			char searchBuffer[GUI::MAX_STR_LEN]{ 0 };
			strncpy(searchBuffer, m_SearchString.c_str(), m_SearchString.size());
			if (ImGui::InputText("Search", searchBuffer, GUI::MAX_STR_LEN))
				m_SearchString = std::string(searchBuffer);

			// TODO: Make dragdrop of all asset types

			if (ImGui::BeginTable("Asset Table", 1, ImGuiTableFlags_Borders))
			{
				auto assets = AssetDatabase::GetAssetHandles();
				for (auto& asset : assets)
				{
					std::string assetIDString = std::to_string(asset.first);

					bool idMatch = assetIDString.find(m_SearchString) != std::string::npos;
					bool pathMatch = asset.second.string().find(m_SearchString) != std::string::npos;

					if (idMatch || pathMatch)
					{
						ImGui::TableNextColumn();

						ImGui::TextWrapped("Asset ID: %s", assetIDString.c_str());
						ImGui::TextWrapped("Asset Import path: %s", asset.second.string().c_str());

						uint32_t assetCount = AssetDatabase::GetAssetReferences(asset.first);
						std::string assetCountString = std::to_string(assetCount);
						ImGui::TextWrapped("Asset references: %s", assetCountString.c_str());

						uint32_t strongCount = AssetDatabase::GetStrongReferences(asset.first);
						std::string strongCountString = std::to_string(strongCount);
						ImGui::TextWrapped("Strong references: %s", strongCountString.c_str());
					}
				}

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}
}