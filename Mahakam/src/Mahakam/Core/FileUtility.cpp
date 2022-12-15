#include "Mahakam/mhpch.h"
#include "FileUtility.h"

#include "SceneManager.h"

#ifndef MH_STANDALONE
#include "Mahakam/Editor/Selection.h"
#endif

namespace Mahakam
{
	void FileUtility::SetProjectDirectory(const Filepath& filepath)
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

			AssetDatabase::ReloadAssets();
		}
	}

	void FileUtility::SetWorkingDirectory(const Filepath& filepath)
	{
		std::filesystem::current_path(filepath);
	}

	Filepath FileUtility::GetWorkingDirectory()
	{
		return std::filesystem::current_path();
	}

	bool FileUtility::ReadFile(const Filepath& filepath, TrivialVector<char>& buffer)
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

	bool FileUtility::Exists(const Filepath& src)
	{
		return std::filesystem::exists(src);
	}

	void FileUtility::CreateDirectories(const Filepath& src)
	{
		if (!std::filesystem::exists(src))
			std::filesystem::create_directories(src);
	}

	Filepath FileUtility::GetCachePath(const Filepath& filepath)
	{
		Filepath importDirectory = CACHE_PATH / filepath.parent_path();

		CreateDirectories(importDirectory);

		return CACHE_PATH / Filepath(filepath.string() + ".cache");
	}

	Filepath FileUtility::GetImportPath(const Filepath& filepath)
	{
		Filepath importPath = std::filesystem::relative(filepath, PROJECT_PATH);

		Filepath importDirectory = IMPORT_PATH / importPath.parent_path();

		CreateDirectories(importDirectory);

		return IMPORT_PATH / Filepath(importPath.string() + ".import");
	}
}