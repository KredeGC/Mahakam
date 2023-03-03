#include "Mahakam/mhpch.h"

#include "Mahakam/Core/Application.h"
#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Log.h"

#include <GLFW/glfw3.h>

#define LINUX_PATH_MAX 4096 + 255 + 1

namespace Mahakam
{
	static bool FileDialogsUtilityCheck()
	{
		FILE* fp;
		char path[LINUX_PATH_MAX];

		fp = popen("which zenity", "r");
		if (fp == NULL)
			MH_ERROR("Null file pointer in FileUtility::OpenFile().\n");

		if (fgets(path, LINUX_PATH_MAX, fp) == NULL)
		{
			MH_WARN("Zenity not found in path!");
			pclose(fp);
			return false;
		}
		pclose(fp);

		return true;
	}

	Filepath FileUtility::OpenFile(const char* filter, const Filepath& basePath)
	{
		if (!FileDialogsUtilityCheck())
			return Filepath();

		const char* file_extension = std::strchr(filter, 0) + 1;

		FILE* fp;
		char path[LINUX_PATH_MAX];

		std::string cmdline = std::string("zenity --file-selection --file-filter=\"") +
			std::string(file_extension) + std::string("\" --title=\"Open File\"");
		fp = popen(cmdline.c_str(), "r");
		if (fp == NULL)
			MH_ERROR("Null file pointer in FileUtility::OpenFile().\n");

		//Check for File dialog cancellation.
		if (fgets(path, LINUX_PATH_MAX, fp) == NULL)
		{
			pclose(fp);
			return Filepath();
		}

		pclose(fp);

		std::string file_choice = std::string(path);
		int endline_pos = file_choice.find_first_of("\n");
		file_choice.resize(endline_pos);
		return file_choice;
	}

	Filepath FileUtility::SaveFile(const char* filter, const Filepath& basePath)
	{
		if (!FileDialogsUtilityCheck())
			return Filepath();

		FILE* fp;
		char path[LINUX_PATH_MAX];
		const char* file_extension = std::strchr(filter, 0) + 1;
		std::string shortened_ext(file_extension);
		shortened_ext.erase(0, 1);

		std::string cmdline = std::string("zenity --file-selection --file-filter=\"") +
			std::string(file_extension) + std::string("\" --filename=\"") + shortened_ext
			+ ("\" --title=\"Save As File\" --save --confirm-overwrite");
		fp = popen(cmdline.c_str(), "r");
		if (fp == NULL)
			MH_ERROR("Null file pointer in FileUtility::SaveFile().\n");

		//Check for File dialog cancellation.
		if (fgets(path, LINUX_PATH_MAX, fp) == NULL)
		{
			pclose(fp);
			return Filepath();
		}

		pclose(fp);

		std::string file_choice = std::string(path);
		int endline_pos = file_choice.find_first_of("\n");
		file_choice.resize(endline_pos);
		return file_choice;
	}

	Filepath FileUtility::OpenDirectory()
	{
		if (!FileDialogsUtilityCheck())
			return Filepath();

		return Filepath();
	}
}