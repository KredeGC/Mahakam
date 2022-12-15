#include "Mahakam/mhpch.h"
#include "Mahakam/Core/FileUtility.h"

#include "Mahakam/Core/Application.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <windows.h>
#include <commdlg.h>
#include <ShlObj.h>
#include <atlbase.h>

namespace Mahakam
{
	struct ComInit
	{
		ComInit() { HRESULT result = CoInitialize(nullptr); }
		~ComInit() { CoUninitialize(); }
	};

	Filepath FileUtility::OpenFile(const char* filter, const Filepath& basePath)
	{
		std::string pathString = (FileUtility::GetWorkingDirectory() / basePath).string();

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
#ifdef MH_HEADLESS
		ofn.hwndOwner = NULL;
#else
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::GetInstance()->GetWindow().GetNativeWindow());
#endif // MH_HEADLESS
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrInitialDir = pathString.c_str();
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return Filepath();
	}

	Filepath FileUtility::SaveFile(const char* filter, const Filepath& basePath)
	{
		std::string pathString = (FileUtility::GetWorkingDirectory() / basePath).string();

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
#ifdef MH_HEADLESS
		ofn.hwndOwner = NULL;
#else
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::GetInstance()->GetWindow().GetNativeWindow());
#endif // MH_HEADLESS
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrInitialDir = pathString.c_str();
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return Filepath();
	}

	Filepath FileUtility::OpenDirectory()
	{
		std::wstring pathString = FileUtility::GetWorkingDirectory().wstring();

		// Initialize COM to be able to use classes like IFileOpenDialog.
		ComInit com;

		// Create an instance of IFileOpenDialog.
		CComPtr<IFileOpenDialog> pFolderDlg;
		if (SUCCEEDED(pFolderDlg.CoCreateInstance(CLSID_FileOpenDialog)))
		{
			// Set options for a filesystem folder picker dialog.
			FILEOPENDIALOGOPTIONS opt{};
			if (SUCCEEDED(pFolderDlg->GetOptions(&opt)))
				pFolderDlg->SetOptions(opt | FOS_PICKFOLDERS | FOS_PATHMUSTEXIST | FOS_FORCEFILESYSTEM);

			// Set default folder
			CComPtr<IShellItem> pDefaultFolder;
			if (SUCCEEDED(SHCreateItemFromParsingName(pathString.c_str(), NULL, IID_PPV_ARGS(&pDefaultFolder.p))))
				pFolderDlg->SetDefaultFolder(pDefaultFolder);

			// Show the dialog modally.
			if (SUCCEEDED(pFolderDlg->Show(nullptr)))
			{
				// Get the path of the selected folder and output it to the console.
				CComPtr<IShellItem> pSelectedItem;
				if (SUCCEEDED(pFolderDlg->GetResult(&pSelectedItem)))
				{
					CComHeapPtr<wchar_t> pPath;
					if (SUCCEEDED(pSelectedItem->GetDisplayName(SIGDN_FILESYSPATH, &pPath)))
						return Filepath(pPath.m_pData);
				}
			}
		}

		return Filepath();
	}
}