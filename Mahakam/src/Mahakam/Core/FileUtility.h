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

		static bool Exists(const std::filesystem::path& src);
		static void CreateDirectories(const std::filesystem::path& src);
		static std::filesystem::path GetCachePath(const std::filesystem::path& filepath);
		static std::filesystem::path GetImportPath(const std::filesystem::path& filepath);

		// Defined by each platform
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}