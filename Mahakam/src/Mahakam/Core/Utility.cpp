#include "Mahakam/mhpch.h"
#include "Utility.h"

namespace Mahakam
{
	bool FileUtility::Exists(const std::filesystem::path& src)
	{
		return std::filesystem::exists(src);
	}

	void FileUtility::CreateDirectories(const std::filesystem::path& src)
	{
		if (!std::filesystem::exists(src))
			std::filesystem::create_directories(src);
	}

	std::filesystem::path FileUtility::GetCachePath(const std::filesystem::path& filepath)
	{
		std::filesystem::path importDirectory = CACHE_PATH / filepath.parent_path();

		CreateDirectories(importDirectory);

		return CACHE_PATH / std::filesystem::path(filepath.string() + ".cache");
	}

	std::filesystem::path FileUtility::GetImportPath(const std::filesystem::path& filepath)
	{
		std::filesystem::path importDirectory = IMPORT_PATH / filepath.parent_path();

		CreateDirectories(importDirectory);

		if (std::filesystem::is_directory(filepath))
			return IMPORT_PATH / filepath.string();
		else
			return IMPORT_PATH / std::filesystem::path(filepath.string() + ".import");
	}
}