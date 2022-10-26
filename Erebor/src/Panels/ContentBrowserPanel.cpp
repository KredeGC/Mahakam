#include "ebpch.h"
#include "ContentBrowserPanel.h"

#include "ImportWizardPanel.h"

#include <unordered_set>

namespace Mahakam::Editor
{
	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_DirectoryIcon = Texture2D::Create("internal/icons/icon-directory.png", { TextureFormat::RGBA8 });
		m_FileIcon = Texture2D::Create("internal/icons/icon-file.png", { TextureFormat::RGBA8 });
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		// TODO: Don't render when in play-mode, unless hovered
		// TODO: Cache the directory structure every second
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

					ImGuiTreeNodeFlags flags = ((m_CurrentDirectory == FileUtility::ASSET_PATH) ? ImGuiTreeNodeFlags_Selected : 0);
					flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;

					bool open = ImGui::TreeNodeEx("assets", flags | ImGuiTreeNodeFlags_DefaultOpen, "%s", "assets");

					if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
						m_CurrentDirectory = FileUtility::ASSET_PATH;

					if (open)
					{
						DirectorySet directories = GetDirectores(FileUtility::ASSET_PATH);

						DrawDirectoryRecursive(FileUtility::ASSET_PATH, directories);
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

	bool ContentBrowserPanel::HasSubDirectories(const std::filesystem::path& path)
	{
		if (FileUtility::Exists(path))
		{
			auto iter = std::filesystem::directory_iterator(path);
			for (auto& directory : iter)
			{
				if (directory.is_directory())
					return true;
			}
		}

		{
			std::filesystem::path importPath = FileUtility::GetImportPath(path);
			if (FileUtility::Exists(importPath))
			{
				auto importIter = std::filesystem::directory_iterator(importPath);
				for (auto& directory : importIter)
				{
					if (directory.is_directory())
						return true;
				}
			}
		}

		return false;
	}

	ContentBrowserPanel::DirectorySet ContentBrowserPanel::GetDirectores(const std::filesystem::path& path)
	{
		DirectorySet directories(Allocator::GetAllocator<std::string>());

		if (FileUtility::Exists(path))
		{
			auto iter = std::filesystem::directory_iterator(path);
			for (auto& directory : iter)
			{
				if (directory.is_directory())
					directories.insert(directory.path().filename().string());
			}
		}

		{
			std::filesystem::path importPath = FileUtility::GetImportPath(path);
			if (FileUtility::Exists(importPath))
			{
				auto importIter = std::filesystem::directory_iterator(importPath);
				for (auto& directory : importIter)
				{
					if (directory.is_directory())
						directories.insert(directory.path().filename().string());
				}
			}
		}

		return directories;
	}

	void ContentBrowserPanel::DrawDirectoryRecursive(const std::filesystem::path& path, const DirectorySet& directories)
	{
		for (const auto& directory : directories)
		{
			std::filesystem::path dirPath = path / directory;

			ImGuiTreeNodeFlags flags = ((m_CurrentDirectory == dirPath) ? ImGuiTreeNodeFlags_Selected : 0);
			flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			if (!HasSubDirectories(dirPath))
				flags |= ImGuiTreeNodeFlags_Leaf;

			bool open = ImGui::TreeNodeEx(dirPath.string().c_str(), flags, "%s", directory.c_str());

			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				m_CurrentDirectory = dirPath;

			if (open)
			{
				DirectorySet childDirectories = GetDirectores(dirPath);
				DrawDirectoryRecursive(dirPath, childDirectories);
				ImGui::TreePop();
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
				// Draw .. directory
				if (path != FileUtility::ASSET_PATH)
				{
					ImGui::TableNextColumn();

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
					ImGui::PushID("..");
					ImGui::ImageButton((ImTextureID)(uintptr_t)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						m_CurrentDirectory = path.parent_path();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
					ImGui::TextWrapped("%s", "..");
					ImGui::PopID();
				}

				// Draw other directories
				if (std::filesystem::exists(path))
				{
					DirectorySet directories = GetDirectores(path);
					for (const auto& directory : directories)
					{
						ImGui::TableNextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
						ImGui::PushID(directory.c_str());
						ImGui::ImageButton((ImTextureID)(uintptr_t)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							m_CurrentDirectory = path / directory;
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
						ImGui::TextWrapped("%s", directory.c_str());
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
				if (std::filesystem::exists(path))
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
				}

				ImGui::EndTable();
			}
		}
	}
}