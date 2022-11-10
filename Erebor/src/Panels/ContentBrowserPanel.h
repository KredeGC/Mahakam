#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class ContentBrowserPanel : EditorWindow
	{
	private:
		using DirectorySet = std::set<std::string, std::less<std::string>, Allocator::BaseAllocator<std::string>>;

		struct DirNode
		{
			std::string Name;
			DirNode* FirstChild;
			DirNode* Next;

			DirNode(const std::string& name, DirNode* first, DirNode* next) :
				Name(name),
				FirstChild(first),
				Next(next) {}
		};

		bool m_Open = true;

		std::filesystem::path m_CurrentDirectory = FileUtility::ASSET_PATH;

		DirNode* m_RootDir;
		DirectorySet m_Directories;
		float m_Time = 1.0f;

		Asset<Texture2D> m_DirectoryIcon;
		Asset<Texture2D> m_FileIcon;

	public:
		ContentBrowserPanel();
		~ContentBrowserPanel();

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;

	private:
		void ClearFileStructure(DirNode* parent);

		void CreateFileStructure(DirNode* parent, const std::filesystem::path& path);

		void CreateDirectories(const std::filesystem::path& path);

		void DrawDirectoryRecursive(DirNode* parent, const std::filesystem::path& path);

		void DrawDirectoryContents(std::filesystem::path path);
	};
}