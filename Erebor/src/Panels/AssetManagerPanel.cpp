#include "ebpch.h"
#include "AssetManagerPanel.h"

namespace Mahakam::Editor
{
	AssetManagerPanel::AssetManagerPanel()
	{

	}

	void AssetManagerPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			ImGui::Begin("Asset Manager", &m_Open);

			if (ImGui::BeginTable("Asset Table", 1, ImGuiTableFlags_Borders))
			{
				auto assets = AssetDatabase::GetAssetHandles();
				for (auto& asset : assets)
				{
					ImGui::TableNextColumn();

					std::string assetIDString = std::to_string(asset.first);
					ImGui::TextWrapped("Asset ID: %s", assetIDString.c_str());
					ImGui::TextWrapped("Asset Import path: %s", asset.second.string().c_str());

					uint32_t assetCount = AssetDatabase::GetAssetReferences(asset.first);
					std::string assetCountString = std::to_string(assetCount);
					ImGui::TextWrapped("Asset references: %s", assetCountString.c_str());

					uint32_t strongCount = AssetDatabase::GetStrongReferences(asset.first);
					std::string strongCountString = std::to_string(strongCount);
					ImGui::TextWrapped("Strong references: %s", strongCountString.c_str());
				}

				ImGui::EndTable();
			}

			ImGui::End();
		}
	}
}