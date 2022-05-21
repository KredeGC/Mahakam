#pragma once

#include <Mahakam.h>

namespace Mahakam::Editor
{
	class ContentBrowserPanel : EditorWindow
	{
	private:
		bool m_Open = true;

		std::filesystem::path m_CurrentDirectory = s_AssetDirectory;

		Asset<Texture2D> m_DirectoryIcon;
		Asset<Texture2D> m_FileIcon;

		inline const static std::filesystem::path s_AssetDirectory = "assets";

	public:
		ContentBrowserPanel();

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;

	private:
		void DrawDirectoryRecursive(std::filesystem::path path);

		void DrawDirectoryContents(std::filesystem::path path);
	};
}