#pragma once

#include <Mahakam/Mahakam.h>

#include <vector>
#include <set>

namespace Mahakam::Editor
{
	class ContentBrowserPanel : EditorWindow
	{
	private:
        static constexpr uint32_t ICON_PADDING = 20;
        static constexpr uint32_t ICON_SIZE = 80;
        
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
		std::string m_ImporterExtension;

		DirNode* m_RootDir;
		DirectorySet m_Directories;
		float m_Time = 2.0f;

		Asset<Texture2D> m_DirectoryIcon;
		Asset<Texture2D> m_FileIcon;
        
        std::vector<Asset<void>> m_CachedAssets;
        UnorderedMap<std::string, Asset<FrameBuffer>> m_Icons;

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
        
        void CreateIcons(const Filepath& path);

		void DrawDirectoryRecursive(DirNode* parent, const std::filesystem::path& path);

		void DrawDirectoryContents(std::filesystem::path path);
	};
}