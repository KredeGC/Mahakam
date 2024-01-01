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

		// Only update every 2 seconds
		if (m_Time >= 2.0f)
		{
			m_Time -= 2.0f;

			ClearFileStructure(m_RootDir);

			CreateFileStructure(m_RootDir, FileUtility::RESOURCE_PATH);

			CreateDirectories(m_CurrentDirectory);
            
            CreateIcons(m_CurrentDirectory);
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

					DrawDirectoryRecursive(m_RootDir, FileUtility::RESOURCE_PATH);

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
		std::filesystem::path importPath = FileUtility::IMPORT_PATH / std::filesystem::relative(path, FileUtility::RESOURCE_PATH);
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

		std::filesystem::path importPath = FileUtility::IMPORT_PATH / std::filesystem::relative(path, FileUtility::RESOURCE_PATH);
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
    
    void ContentBrowserPanel::CreateIcons(const Filepath& path)
    {
        return; // TODO: Fix all this stuff for custom icons
        
        //m_Icons.clear();
        
        Filepath importDirectory = FileUtility::IMPORT_PATH / std::filesystem::relative(m_CurrentDirectory, FileUtility::RESOURCE_PATH);
        for (auto& file : std::filesystem::directory_iterator(importDirectory))
        {
            if (std::filesystem::is_directory(file)) continue;
            if (m_Icons.find(file.path().string()) != m_Icons.end()) continue;
            
            auto info = AssetDatabase::ReadAssetInfo(file.path());
            auto importer = AssetDatabase::GetAssetImporter(info.Extension);
            
            if (!importer) continue;
            
            Asset<void> asset(ResourceRegistry::GetImportInfo(file.path()).ID);
            
            if (!asset) continue;
            
            // Make sure it's always loaded
            m_CachedAssets.push_back(asset);
            
            // Render the icon
            FrameBufferProps props;
            props.Width = ICON_SIZE;
            props.Height = ICON_SIZE;
            props.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{ TextureFormat::RGBA8 };
            props.DontUseDepth = true;
            
            Asset<FrameBuffer> framebuffer = FrameBuffer::Create(props);
            
            framebuffer->Bind();
            
            GL::Clear();
            
            bool useIcon = importer->OnIconRender(asset);
            
            framebuffer->Unbind();
            
            if (useIcon)
				m_Icons.insert({ file.path().string(), framebuffer });
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
			CreateIcons(m_CurrentDirectory);
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

	void ContentBrowserPanel::DrawDirectoryContents(std::filesystem::path basePath)
	{
		static constexpr float cellSize = ICON_PADDING + ICON_SIZE;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int numColumns = glm::max((int)(panelWidth / cellSize), 1);

		// Directories first
		if (ImGui::CollapsingHeader("Directories", ImGuiTreeNodeFlags_SpanAvailWidth))
		{
			if (ImGui::BeginTable("Directory Divisor", numColumns))
			{
				// Draw .. directory
				if (basePath != FileUtility::RESOURCE_PATH)
				{
					ImGui::TableNextColumn();

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
					ImGui::PushID("..");
					ImGui::ImageButton((ImTextureID)(uintptr_t)m_DirectoryIcon->GetRendererID(), { ICON_SIZE, ICON_SIZE }, { 0, 1 }, { 1, 0 });

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_CurrentDirectory = basePath.parent_path();
						CreateDirectories(m_CurrentDirectory);
			            CreateIcons(m_CurrentDirectory);
					}

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
					ImGui::TextWrapped("%s", "..");
					ImGui::PopID();
				}

				// Draw other directories
				if (std::filesystem::exists(basePath))
				{
					for (const auto& directory : m_Directories)
					{
						ImGui::TableNextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
						ImGui::PushID(directory.c_str());
						ImGui::ImageButton((ImTextureID)(uintptr_t)m_DirectoryIcon->GetRendererID(), { ICON_SIZE, ICON_SIZE }, { 0, 1 }, { 1, 0 });

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							m_CurrentDirectory = basePath / directory;
							CreateDirectories(m_CurrentDirectory);
			                CreateIcons(m_CurrentDirectory);
						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
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
				std::filesystem::path importDirectory = FileUtility::IMPORT_PATH / std::filesystem::relative(basePath, FileUtility::RESOURCE_PATH);
				if (std::filesystem::exists(importDirectory))
				{
					for (auto& file : std::filesystem::directory_iterator(importDirectory))
					{
						std::filesystem::path importPath = file.path();
						if (!file.is_directory() && importPath.extension() == ".import")
						{
							std::string pathName = importPath.stem().string();

							ImGui::TableNextColumn();

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
							ImGui::PushID(importPath.string().c_str());
							//ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { ICON_SIZE, ICON_SIZE }, { 0, 1 }, { 1, 0 });
                            
                            auto iconIter = m_Icons.find(file.path().string());
                            if (iconIter != m_Icons.end())
                                ImGui::ImageButton((ImTextureID)(uintptr_t)iconIter->second->GetColorTexture(0)->GetRendererID(), { ICON_SIZE, ICON_SIZE }, { 0, 1 }, { 1, 0 });
                            else
							    ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { ICON_SIZE, ICON_SIZE }, { 0, 1 }, { 1, 0 });

							if (ImGui::BeginDragDropSource())
							{
								ResourceRegistry::ImportInfo info = ResourceRegistry::GetImportInfo(importPath);

								ImGui::SetDragDropPayload(info.Type.c_str(), &info.ID, sizeof(ResourceRegistry::AssetID));

								ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { ICON_SIZE, ICON_SIZE }, { 0, 1 }, { 1, 0 });
								ImGui::Text("%s", pathName.c_str());

								ImGui::EndDragDropSource();
							}

							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								ResourceRegistry::ImportInfo info = ResourceRegistry::GetImportInfo(importPath);

								ImportWizardPanel::ImportOpen(importPath, info.Type);
							}

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
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
				if (std::filesystem::exists(basePath))
				{
					for (auto& file : std::filesystem::directory_iterator(basePath))
					{
						if (!file.is_directory())
						{
							std::string pathName = file.path().filename().string();

							ImGui::TableNextColumn();

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
							ImGui::PushID(file.path().string().c_str());
							ImGui::ImageButton((ImTextureID)(uintptr_t)m_FileIcon->GetRendererID(), { ICON_SIZE, ICON_SIZE }, { 0, 1 }, { 1, 0 }, -1, { 0, 0, 0, 0 }, { 0.48f, 0.5f, 0.53f, 1 });

							// TODO: Add ImGui::BeginDragDropSource()

							if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								m_ImporterExtension = file.path().extension().string();

								auto ext = ResourceRegistry::GetAssetImporterExtension(m_ImporterExtension);
								auto iter = ext.first;
								auto iterEnd = ext.second;

								if (iter != iterEnd)
								{
									auto props = iter->second->GetImporterProps();
									if (++iter != iterEnd)
									{
										ImGui::OpenPopup("ImportAssetSelectPopup");
									}
									else
									{
										ImportWizardPanel::ResourceOpen(file.path(), props.Extension);
									}
								}
							}

							// Draw a popup to choose importer
							if (ImGui::BeginPopup("ImportAssetSelectPopup"))
							{
								ImGui::Text("Import asset as...");

								auto ext = ResourceRegistry::GetAssetImporterExtension(m_ImporterExtension);
								auto iter = ext.first;
								auto iterEnd = ext.second;
								while (iter != iterEnd)
								{
									const auto& props = iter->second->GetImporterProps();
									if (ImGui::MenuItem(props.Name.c_str()))
										ImportWizardPanel::ResourceOpen(file.path(), props.Extension);

									++iter;
								}

								ImGui::EndPopup();
							}

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ICON_PADDING / 2);
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
				const auto& importers = ResourceRegistry::GetAssetImporters();

				for (const auto& kv : importers)
				{
					const auto& props = kv.second->GetImporterProps();
					if (props.CreateMenu)
					{
						if (ImGui::MenuItem(props.Name.c_str()))
						{
							std::filesystem::path filepath = m_CurrentDirectory / props.Name;
							std::filesystem::path importPath = FileUtility::GetImportPath(filepath, props.Extension);
							ImportWizardPanel::ImportOpen(importPath, props.Extension);
						}
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Reimport all assets"))
			{
				// TODO: For each registered importer and file
				// Call CreateAsset
				// Call Save
			}

			ImGui::EndPopup();
		}
	}
}