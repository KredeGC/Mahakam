#include "ebpch.h"
#include "ContentBrowserPanel.h"

#include "ImportWizardPanel.h"

#include <unordered_set>

namespace Mahakam::Editor
{
	ContentBrowserPanel::ContentBrowserPanel()
		: m_Directories(Allocator::GetAllocator<std::string>())
	{
		m_DirectoryIcon = Texture2D::Create("internal/icons/icon-directory.png", { TextureFormat::RGBA8 });
		m_FileIcon = Texture2D::Create("internal/icons/icon-file.png", { TextureFormat::RGBA8 });

		m_RootDir = Allocator::New<DirNode>("assets", nullptr, nullptr);
	}

	ContentBrowserPanel::~ContentBrowserPanel()
	{
		Allocator::Delete<DirNode>(m_RootDir);
	}

	void ContentBrowserPanel::OnUpdate(Timestep dt)
	{
		m_Time += dt;

		// Only update every 3 seconds
		if (m_Time >= 2.0f)
		{
			m_Time -= 2.0f;

			ClearFileStructure(m_RootDir);

			CreateFileStructure(m_RootDir, FileUtility::ASSET_PATH);

			CreateDirectories(m_CurrentDirectory);
		}
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

					DrawDirectoryRecursive(m_RootDir, FileUtility::ASSET_PATH);

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

	void ContentBrowserPanel::ClearFileStructure(DirNode* parent)
	{
		DirNode* current = parent->FirstChild;
		while (current)
		{
			DirNode* next = current->Next;

			ClearFileStructure(current);

			Allocator::Delete<DirNode>(current);

			current = next;
		}
	}

	void ContentBrowserPanel::CreateFileStructure(DirNode* parent, const std::filesystem::path& path)
	{
		DirectorySet directories(Allocator::GetAllocator<std::string>());

		// Search the "assets" path
		if (FileUtility::Exists(path))
		{
			auto iter = std::filesystem::directory_iterator(path);
			for (auto& directory : iter)
			{
				if (directory.is_directory())
					directories.insert(directory.path().filename().string());
			}
		}

		// Search the "imports/assets" path
		std::filesystem::path importPath = FileUtility::IMPORT_PATH / std::filesystem::relative(path, FileUtility::PROJECT_PATH);
		if (FileUtility::Exists(importPath))
		{
			auto importIter = std::filesystem::directory_iterator(importPath);
			for (auto& directory : importIter)
			{
				if (directory.is_directory())
					directories.insert(directory.path().filename().string());
			}
		}

		// Add directories as children to the parent node
		DirNode* sibling = nullptr;
		for (auto& dir : directories)
		{
			DirNode* node = Allocator::New<DirNode>(dir, nullptr, nullptr);

			if (sibling)
				sibling->Next = node;
			else
				parent->FirstChild = node;

			sibling = node;

			CreateFileStructure(node, path / dir);
		}
	}

	void ContentBrowserPanel::CreateDirectories(const std::filesystem::path& path)
	{
		m_Directories.clear();

		if (FileUtility::Exists(path))
		{
			auto iter = std::filesystem::directory_iterator(path);
			for (auto& directory : iter)
			{
				if (directory.is_directory())
					m_Directories.insert(directory.path().filename().string());
			}
		}

		std::filesystem::path importPath = FileUtility::IMPORT_PATH / std::filesystem::relative(path, FileUtility::PROJECT_PATH);
		if (FileUtility::Exists(importPath))
		{
			auto importIter = std::filesystem::directory_iterator(importPath);
			for (auto& directory : importIter)
			{
				if (directory.is_directory())
					m_Directories.insert(directory.path().filename().string());
			}
		}
	}

	void ContentBrowserPanel::DrawDirectoryRecursive(DirNode* parent, const std::filesystem::path& path)
	{
		ImGuiTreeNodeFlags flags = ((m_CurrentDirectory == path) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (!parent->FirstChild)
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (parent == m_RootDir)
			flags |= ImGuiTreeNodeFlags_DefaultOpen;

		bool open = ImGui::TreeNodeEx(path.string().c_str(), flags, "%s", parent->Name.c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		{
			m_CurrentDirectory = path;
			CreateDirectories(m_CurrentDirectory);
		}

		if (open)
		{
			DirNode* current = parent->FirstChild;
			while (current)
			{
				DrawDirectoryRecursive(current, path / current->Name);

				current = current->Next;
			}

			ImGui::TreePop();
		}
	}

	void ContentBrowserPanel::DrawDirectoryContents(std::filesystem::path path)
	{
		float padding = 20.0f;
		float thumbnailSize = 80.0f;
		float cellSize = padding + thumbnailSize;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int numColumns = glm::max((int)(panelWidth / cellSize), 1);

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
					{
						m_CurrentDirectory = path.parent_path();
						CreateDirectories(m_CurrentDirectory);
					}

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
					ImGui::TextWrapped("%s", "..");
					ImGui::PopID();
				}

				// Draw other directories
				if (std::filesystem::exists(path))
				{
					for (const auto& directory : m_Directories)
					{
						ImGui::TableNextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding / 2);
						ImGui::PushID(directory.c_str());
						ImGui::ImageButton((ImTextureID)(uintptr_t)m_DirectoryIcon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							m_CurrentDirectory = path / directory;
							CreateDirectories(m_CurrentDirectory);
						}

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
				std::filesystem::path importPath = FileUtility::IMPORT_PATH / std::filesystem::relative(path, FileUtility::PROJECT_PATH);
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
		
		if (ImGui::BeginPopupContextWindow("ContentBrowser Empty RMB", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::BeginMenu("Create"))
			{
				const auto& importers = AssetDatabase::GetAssetImporters();

				for (const auto& kv : importers)
				{
					const auto& props = kv->GetImporterProps();
					if (props.CreateMenu)
					{
						if (ImGui::MenuItem(props.Extension.c_str()))
						{
							std::string filename = "Asset" + props.Extension;
							std::filesystem::path filepath = m_CurrentDirectory / filename;
							std::filesystem::path importPath = FileUtility::GetImportPath(filepath);
							ImportWizardPanel::ImportAsset(filepath, props.Extension, importPath);
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}
}