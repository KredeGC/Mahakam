#include "Mahakam/mhpch.h"
#include "FileUtility.h"

#ifndef MH_STANDALONE
#include "Mahakam/Editor/Selection.h"
#endif

namespace Mahakam
{
	void FileUtility::SetProjectDirectory(const Filepath& filepath) noexcept
	{
		// Only reload if the path has changed
		if (filepath != PROJECT_PATH)
		{
#ifndef MH_STANDALONE
			Editor::Selection::SetSelectedEntity({});
#endif

			// Set the project paths
			PROJECT_PATH = filepath;
			ASSET_PATH = filepath / "assets";
			IMPORT_PATH = filepath / "import";
			RESOURCE_PATH = filepath / "resource";

			AssetDatabase::ReloadAssets();
		}
	}

	void FileUtility::SetWorkingDirectory(const Filepath& filepath) noexcept
	{
		std::filesystem::current_path(filepath);
	}

	Filepath FileUtility::GetWorkingDirectory() noexcept
	{
		return std::filesystem::current_path();
	}

	bool FileUtility::ReadFile(const Filepath& filepath, TrivialVector<char>& buffer) noexcept
	{
		std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);

		if (!ifs)
			return false;

		auto end = ifs.tellg();
		ifs.seekg(0, std::ios::beg);

		auto size = size_t(end - ifs.tellg());

		if (size == 0) // avoid undefined behavior 
			return false;

		buffer.resize(size);

		if (!ifs.read(buffer.data(), buffer.size()))
			return false;

		return true;
	}

	size_t FileUtility::Hash(const Filepath& filepath) noexcept
	{
		std::string filepathStr = filepath.generic_string();

		return Hash(filepathStr.c_str(), filepathStr.size());
	}

	bool FileUtility::Exists(const Filepath& src) noexcept
	{
		return std::filesystem::exists(src);
	}

	void FileUtility::CreateDirectories(const Filepath& src) noexcept
	{
		if (!std::filesystem::exists(src))
			std::filesystem::create_directories(src);
	}

	Filepath FileUtility::Relative(const Filepath& filepath) noexcept
	{
		return std::filesystem::relative(filepath, PROJECT_PATH);
	}

	Filepath FileUtility::GetCachePath(const Filepath& filepath) noexcept
	{
		Filepath importDirectory = CACHE_PATH / filepath.parent_path();

		CreateDirectories(importDirectory);

		return CACHE_PATH / Filepath(filepath.string() + ".cache");
	}

	Filepath FileUtility::GetImportPath(const Filepath& filepath, const std::string& extension) noexcept
	{
		Filepath importPath = std::filesystem::relative(filepath, RESOURCE_PATH);

		Filepath importDirectory = IMPORT_PATH / importPath.parent_path();

		CreateDirectories(importDirectory);

		return IMPORT_PATH / Filepath(importPath.string() + extension + ".import");
	}
}