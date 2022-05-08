#include "ebpch.h"
#include "ContentBrowserPanel.h"

namespace Mahakam::Editor
{
	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_DirectoryIcon = Texture2D::Create("res/internal/icons/icon-directory.png", { TextureFormat::RGBA8 });
		m_FileIcon = Texture2D::Create("res/internal/icons/icon-file.png", { TextureFormat::RGBA8 });
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			ImGui::Begin("Content Browser", &m_Open);

			if (ImGui::BeginTable("Content Table", 2, ImGuiTableFlags_Resizable))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				ImGuiTreeNodeFlags flags = ((m_CurrentDirectory == s_AssetDirectory) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

				bool open = ImGui::TreeNodeEx(s_AssetDirectory.string().c_str(), flags | ImGuiTreeNodeFlags_DefaultOpen, s_AssetDirectory.string().c_str());

				if (ImGui::IsItemClicked())
					m_CurrentDirectory = s_AssetDirectory;

				if (open)
				{
					DrawDirectoryRecursive(s_AssetDirectory);
					ImGui::TreePop();
				}

				ImGui::TableNextColumn();

				DrawDirectoryContents(m_CurrentDirectory);

				ImGui::EndTable();
			}

			ImGui::End();
		}
	}

	void ContentBrowserPanel::DrawDirectoryRecursive(std::filesystem::path path)
	{
		auto iter = std::filesystem::directory_iterator(path);

		for (auto& directory : iter)
		{
			if (directory.is_directory())
			{
				std::filesystem::path path = directory.path();
				std::string pathName = directory.path().filename().string();

				ImGuiTreeNodeFlags flags = ((m_CurrentDirectory == path) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

				bool open = ImGui::TreeNodeEx(path.string().c_str(), flags, pathName.c_str());

				if (ImGui::IsItemClicked())
					m_CurrentDirectory = path;

				if (open)
				{
					DrawDirectoryRecursive(path);
					ImGui::TreePop();
				}
			}
		}
	}

	void ContentBrowserPanel::DrawDirectoryContents(std::filesystem::path path)
	{
		float padding = 16.0f;
		float thumbnailSize = 96.0f;
		float cellSize = padding + thumbnailSize;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int numColumns = (int)(panelWidth / cellSize);

		ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

		if (numColumns > 0 && ImGui::BeginTable("File Table", numColumns))
		{
			// Directories first
			for (auto& file : std::filesystem::directory_iterator(path))
			{
				if (file.is_directory())
				{
					std::string pathName = file.path().filename().string();

					ImGui::TableNextColumn();

					ImGui::PushID(file.path().string().c_str());
					ImGui::ImageButton((ImTextureID)(uintptr_t)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_CurrentDirectory = file.path();
					}
					ImGui::TextWrapped(pathName.c_str());
					ImGui::PopID();
				}
			}

			// Files second
			for (auto& file : std::filesystem::directory_iterator(path))
			{
				if (!file.is_directory())
				{
					std::string pathName = file.path().filename().string();

					ImVec4 col = { 1, 1, 1, 1 };
					std::string importPath = "res/" + file.path().string() + ".yaml";
					if (!std::filesystem::exists(importPath))
						col = { 0.48f, 0.5f, 0.53f, 1 };

					ImGui::TableNextColumn();

					ImGui::PushID(file.path().string().c_str());
					ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }, -1, { 0, 0, 0, 0 }, col);
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						Ref<Sound> sound = Sound::Create(file.path().string());
						AssetDatabase::SaveAsset(sound, file.path().extension().string(), importPath);
					}
					ImGui::TextWrapped(pathName.c_str());
					ImGui::PopID();
				}
			}

			ImGui::EndTable();
		}

		ImGui::PopStyleColor();
	}
}