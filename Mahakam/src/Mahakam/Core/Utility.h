#pragma once

#include <string>
#include <filesystem>

namespace Mahakam
{
	class FileUtility
	{
	public:
		inline static const std::filesystem::path CACHE_PATH = "cache";
		inline static const std::filesystem::path IMPORT_PATH = "import";
		inline static const std::filesystem::path ASSET_PATH = "assets";

		inline static bool Exists(const std::filesystem::path& src)
		{
			return std::filesystem::exists(src);
		}

		inline static void CreateDirectories(const std::filesystem::path& src)
		{
			if (!std::filesystem::exists(src))
				std::filesystem::create_directories(src);
		}

		inline static std::filesystem::path GetCachePath(const std::filesystem::path& filepath)
		{
			std::filesystem::path importDirectory = CACHE_PATH / filepath.parent_path();

			CreateDirectories(importDirectory);

			return CACHE_PATH / std::filesystem::path(filepath.string() + ".cache");
		}

		inline static std::filesystem::path GetImportPath(const std::filesystem::path& filepath)
		{
			std::filesystem::path importDirectory = IMPORT_PATH / filepath.parent_path();

			CreateDirectories(importDirectory);

			if (std::filesystem::is_directory(filepath))
				return IMPORT_PATH / filepath.string();
			else
				return IMPORT_PATH / std::filesystem::path(filepath.string() + ".import");
		}

		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}