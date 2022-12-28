#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace Mahakam
{
	struct WindowProps
	{
		std::string Title;
		std::filesystem::path Iconpath;
		uint32_t Width, Height;

		WindowProps(const std::string& title = "Mahakam", const std::filesystem::path& iconpath = "", uint32_t width = 1600, uint32_t height = 900)
			: Title(title), Iconpath(iconpath), Width(width), Height(height) {}
	};
}