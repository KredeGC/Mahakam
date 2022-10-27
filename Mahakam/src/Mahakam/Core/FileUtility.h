#pragma once

#include <string>
#include <filesystem>

namespace Mahakam
{
	class FileUtility
	{
	public:
		inline static std::filesystem::path PROJECT_PATH = std::filesystem::current_path();
		inline static std::filesystem::path CACHE_PATH = "cache";
		inline static std::filesystem::path IMPORT_PATH = "import";
		inline static std::filesystem::path ASSET_PATH = "assets";

		static void SetProjectDirectory(const std::filesystem::path& filepath);

		static void SetWorkingDirectory(const std::filesystem::path& filepath);
		static std::filesystem::path GetWorkingDirectory();

		static bool Exists(const std::filesystem::path& src);
		static void CreateDirectories(const std::filesystem::path& src);
		static std::filesystem::path GetCachePath(const std::filesystem::path& filepath);
		static std::filesystem::path GetImportPath(const std::filesystem::path& filepath);

		// Defined by each platform
		static std::filesystem::path OpenFile(const char* filter, const std::filesystem::path& basePath);
		static std::filesystem::path SaveFile(const char* filter, const std::filesystem::path& basePath);
		static std::filesystem::path OpenDirectory();
	};
}