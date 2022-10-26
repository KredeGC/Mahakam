#pragma once

#include "Mahakam/Core/Core.h"

#include <filesystem>

#if MH_PLATFORM_WINDOWS
#include <windows.h>
#elif MH_PLATFORM_LINUX
#include <dlfcn.h>
#endif

namespace Mahakam
{
	class SharedLibrary
	{
	public:
		static constexpr int NUM_FUNC_PTRS = 111;

	private:
		std::filesystem::path m_Filepath = nullptr;
#if defined(MH_PLATFORM_WINDOWS)
		HINSTANCE m_Handle = 0;
#elif defined(MH_PLATFORM_LINUX)
		void* m_Handle = nullptr;
#endif

		inline static bool s_Initialized = false;
		inline static FuncPtr s_FuncPointers[NUM_FUNC_PTRS];
		inline static int s_FuncPointerCounter = 0;

	public:
		SharedLibrary() = default;

		SharedLibrary(const std::filesystem::path& filepath);

		~SharedLibrary();

		void Load();
		void Unload();

		template<typename R, typename ...Args>
		auto GetFunction(const char* name)
		{
#if defined(MH_PLATFORM_WINDOWS)
			return (R(*)(Args...))GetProcAddress(m_Handle, name);
#elif defined(MH_PLATFORM_LINUX)
			return (R(*)(Args...))dlsym(m_Handle, name);
#endif
		}

		static void AddExportFunction(FuncPtr funcPtr);

		static void ExportFuncPointers();
		static void ImportFuncPointers(FuncPtr ptrs[NUM_FUNC_PTRS]);
	};
}