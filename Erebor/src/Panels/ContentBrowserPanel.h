#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class ContentBrowserPanel : EditorWindow
	{
	private:
		bool m_Open = true;

		std::filesystem::path m_CurrentDirectory = FileUtility::ASSET_PATH;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;

		using DirectorySet = std::set<std::string, std::less<std::string>, Allocator::BaseAllocator<std::string>>;

	public:
		ContentBrowserPanel();

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

	private:
		bool HasSubDirectories(const std::filesystem::path& path);

		DirectorySet GetDirectores(const std::filesystem::path& path);

		void DrawDirectoryRecursive(const std::filesystem::path& path, const DirectorySet& directories);

		void DrawDirectoryContents(std::filesystem::path path);
	};
}