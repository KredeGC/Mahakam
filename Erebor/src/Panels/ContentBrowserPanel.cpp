#include "ebpch.h"
#include "ContentBrowserPanel.h"

#include "ImportWizardPanel.h"

namespace Mahakam::Editor
{
	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_DirectoryIcon = Asset<Texture2D>(Texture2D::Create("internal/icons/icon-directory.png", { TextureFormat::RGBA8 }));
		m_FileIcon = Asset<Texture2D>(Texture2D::Create("internal/icons/icon-file.png", { TextureFormat::RGBA8 }));
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		if (m_Open)
		{
			if (ImGui::Begin("Content Browser", &m_Open))
			{
				ImVec2 size = ImGui::GetContentRegionAvail();

				if (ImGui::BeginTable("Content Table", 2, ImGuiTableFlags_Resizable, size))
				{
					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::BeginChild("File Browser Child", { 0, -2 }, false);

					ImGuiTreeNodeFlags flags = ((m_CurrentDirectory == s_AssetDirectory) ? ImGuiTreeNodeFlags_Selected : 0);
					flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;

					std::string fileString = s_AssetDirectory.string();
					const char* fileCStr = fileString.c_str();

					bool open = ImGui::TreeNodeEx(fileCStr, flags | ImGuiTreeNodeFlags_DefaultOpen, "%s", fileCStr);

					if (ImGui::IsItemClicked())
						m_CurrentDirectory = s_AssetDirectory;

					if (open)
					{
						DrawDirectoryRecursive(s_AssetDirectory);
						ImGui::TreePop();
					}

					ImGui::EndChild();

					ImGui::TableNextColumn();

					ImGui::BeginChild("Content Browser Child", { 0, -2 }, false);

					DrawDirectoryContents(m_CurrentDirectory);

					ImGui::EndChild();

					ImGui::EndTable();
				}
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

				ImGuiTreeNodeFlags flags = ((m_CurrentDirectory == path) ? ImGuiTreeNodeFlags_Selected : 0);
				flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;

				bool open = ImGui::TreeNodeEx(path.string().c_str(), flags, "%s", pathName.c_str());

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
		float padding = 20.0f;
		float thumbnailSize = 80.0f;
		float cellSize = padding + thumbnailSize;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int numColumns = (int)(panelWidth / cellSize);
		if (numColumns < 1)
			numColumns = 1;

		// Directories first
		if (ImGui::CollapsingHeader("Directories", ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			if (ImGui::BeginTable("Directory Divisor", numColumns))
			{
				for (auto& file : std::filesystem::directory_iterator(path))
				{
					if (file.is_directory())
					{
						std::string pathName = file.path().filename().string();

						ImGui::TableNextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
						ImGui::PushID(file.path().string().c_str());
						ImGui::ImageButton((ImTextureID)(uintptr_t)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							m_CurrentDirectory = file.path();
						}
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
						ImGui::TextWrapped("%s", pathName.c_str());
						ImGui::PopID();
					}
				}

				ImGui::EndTable();
			}
		}

		// Import files second
		if (ImGui::CollapsingHeader("Imported Assets", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginTable("Import Divisor", numColumns))
			{
				std::filesystem::path importPath = FileUtility::GetImportPath(path);
				if (std::filesystem::exists(importPath))
				{
					for (auto& file : std::filesystem::directory_iterator(importPath))
					{
						std::filesystem::path importPath = file.path();
						if (!file.is_directory() && importPath.extension() == ".import")
						{
							std::string pathName = importPath.stem().string();

							ImGui::TableNextColumn();

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
							ImGui::PushID(importPath.string().c_str());
							ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

							if (ImGui::BeginDragDropSource())
							{
								AssetDatabase::AssetInfo info = AssetDatabase::ReadAssetInfo(importPath);

								std::string importString = importPath.string();
								const char* importBuffer = importString.c_str();

								ImGui::SetDragDropPayload(info.Extension.c_str(), importBuffer, strlen(importBuffer) + 1);
								
								ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
								ImGui::TextWrapped("%s", pathName.c_str());

								ImGui::EndDragDropSource();
							}

							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								AssetDatabase::AssetInfo info = AssetDatabase::ReadAssetInfo(importPath);

								ImportWizardPanel::ImportAsset(info.Filepath, info.Extension, importPath);
							}
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
							ImGui::TextWrapped("%s", pathName.c_str());
							ImGui::PopID();
						}
					}
				}

				ImGui::EndTable();
			}
		}

		// Raw files last
		if (ImGui::CollapsingHeader("Files", ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginTable("Raw Divisor", numColumns))
			{
				for (auto& file : std::filesystem::directory_iterator(path))
				{
					if (!file.is_directory())
					{
						std::string pathName = file.path().filename().string();

						ImGui::TableNextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
						ImGui::PushID(file.path().string().c_str());
						ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }, -1, { 0, 0, 0, 0 }, { 0.48f, 0.5f, 0.53f, 1 });
						
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							std::filesystem::path importPath = FileUtility::GetImportPath(file.path());
							ImportWizardPanel::ImportAsset(file.path(), file.path().extension().string(), importPath);
						}
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
						ImGui::TextWrapped("%s", pathName.c_str());
						ImGui::PopID();
					}
				}

				ImGui::EndTable();
			}
		}
	}
}