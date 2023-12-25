#pragma once

#include "Types.h"

#include <string>
#include <filesystem>

namespace Mahakam
{
	typedef std::filesystem::path Filepath;

	class FileUtility
	{
	public:
		inline static Filepath PROJECT_PATH = std::filesystem::current_path();
		inline static Filepath CACHE_PATH = "cache";
		inline static Filepath RESOURCE_PATH = "resource";
		inline static Filepath IMPORT_PATH = "import";
		inline static Filepath ASSET_PATH = "assets";

		inline static const std::string AssetExtension = ".asset";

		static void SetProjectDirectory(const Filepath& filepath);

		static void SetWorkingDirectory(const Filepath& filepath);
		static Filepath GetWorkingDirectory();

		static bool ReadFile(const Filepath& filepath, TrivialVector<char>& buffer);

		static bool Exists(const Filepath& src);
		static void CreateDirectories(const Filepath& src);
		static Filepath GetCachePath(const Filepath& filepath);
		static Filepath GetImportPath(const Filepath& filepath, const std::string& extension);
		static Filepath GetAssetPath(const Filepath& filepath);

		// Defined by each platform
		static Filepath OpenFile(const char* filter, const Filepath& basePath);
		static Filepath SaveFile(const char* filter, const Filepath& basePath);
		static Filepath OpenDirectory();
	};
}