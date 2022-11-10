#include "Mahakam/mhpch.h"
#include "FileUtility.h"

#include "SceneManager.h"

#ifndef MH_STANDALONE
#include "Mahakam/Editor/Selection.h"
#endif

namespace Mahakam
{
	void FileUtility::SetProjectDirectory(const std::filesystem::path& filepath)
	{
		// Only reload if the path has changed
		if (filepath != PROJECT_PATH)
		{
#ifndef MH_STANDALONE
			Editor::Selection::SetSelectedEntity({});
#endif

			// Reset the scene
			Ref<Scene> scene = Scene::Create();
			SceneManager::SetActiveScene(scene);

			// Set the project paths
			PROJECT_PATH = filepath;
			ASSET_PATH = std::filesystem::relative(filepath / "assets", GetWorkingDirectory());
			IMPORT_PATH = std::filesystem::relative(filepath / "import", GetWorkingDirectory());

			AssetDatabase::ReloadAssetImports();
		}
	}

	void FileUtility::SetWorkingDirectory(const std::filesystem::path& filepath)
	{
		std::filesystem::current_path(filepath);
	}

	std::filesystem::path FileUtility::GetWorkingDirectory()
	{
		return std::filesystem::current_path();
	}

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
		std::filesystem::path importPath = std::filesystem::relative(filepath, PROJECT_PATH);

		std::filesystem::path importDirectory = IMPORT_PATH / importPath.parent_path();

		CreateDirectories(importDirectory);

		return IMPORT_PATH / std::filesystem::path(importPath.string() + ".import");
	}
}