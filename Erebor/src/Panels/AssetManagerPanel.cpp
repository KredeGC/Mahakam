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

					uint32_t useCount = AssetDatabase::GetAssetReferences(asset.first);
					std::string useCountString = std::to_string(useCount);
					ImGui::TextWrapped("Current references: %s", useCountString.c_str());
				}

				ImGui::EndTable();
			}

			ImGui::End();
		}
	}
}