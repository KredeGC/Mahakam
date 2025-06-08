#include "Mahakam/mhpch.h"
#include "AssetManagerPanel.h"

#include "Mahakam/ImGui/GUI.h"

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
			const auto& importers = ResourceRegistry::GetAssetImporters();
			std::vector<std::string> extensions;
			for (auto& importer : importers)
				extensions.push_back(importer.first);

			std::filesystem::path search = m_SearchString;
			if (GUI::DrawDragDropAssetField("Search", extensions, search))
				m_SearchString = search.string();

			// List all assets within search field
			if (ImGui::BeginTable("Asset Table", 1, ImGuiTableFlags_Borders))
			{
				auto& resources = ResourceRegistry::GetImports();

				struct AssetInfo
				{
					AssetDatabase::AssetID ID;
					std::filesystem::path Filepath;
					std::string Type;
					size_t RefCount;

					AssetInfo(AssetDatabase::AssetID id, const std::filesystem::path& filepath, const std::string& type, size_t ref) noexcept :
						ID(id),
						Filepath(filepath),
						Type(type),
						RefCount(ref) {}
				};

				std::vector<AssetInfo> assets;
				assets.reserve(resources.size());

				for (auto& resource : resources)
					assets.emplace_back(resource.first, FileUtility::Relative(resource.second.Filepath), resource.second.Type, AssetDatabase::GetAssetReferences(resource.first));

				std::sort(assets.begin(), assets.end(), [](AssetInfo& a, AssetInfo& b) { return a.RefCount > b.RefCount; });

				for (auto& asset : assets)
				{
					std::string assetIDString = std::to_string(asset.ID);

					bool idMatch = assetIDString.find(m_SearchString) != std::string::npos;
					bool typeMatch = asset.Type.find(m_SearchString) != std::string::npos;
					bool pathMatch = asset.Filepath.string().find(m_SearchString) != std::string::npos;

					if (idMatch || typeMatch || pathMatch)
					{
						ImGui::TableNextColumn();

						ImGui::TextWrapped("ID: %s", assetIDString.c_str());
						ImGui::TextWrapped("Type: %s", asset.Type.c_str());
						ImGui::TextWrapped("Import path: %s", asset.Filepath.string().c_str());

						std::string assetCountString = std::to_string(asset.RefCount);
						ImGui::TextWrapped("References: %s", assetCountString.c_str());
					}
				}

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}
}