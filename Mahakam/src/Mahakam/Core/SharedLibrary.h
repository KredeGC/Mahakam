#pragma once

#include <filesystem>

#if MH_PLATFORM_WINDOWS
#include <windows.h>
#elif MH_PLATFORM_LINUX
#include <dlfcn.h>
#endif

// Function declarations which should be reachable in DLLs
#ifndef MH_STANDALONE
#define MH_DECLARE_FUNC_LINE2(line, func, returnType, ...) static returnType (*func)(__VA_ARGS__); \
	inline static uint8_t generated_##line = (::Mahakam::SharedLibrary::AddExportFunction((void**)&func), 0);
#define MH_DECLARE_FUNC_LINE(line, func, returnType, ...) MH_DECLARE_FUNC_LINE2(line, func, returnType, __VA_ARGS__)

#define MH_DECLARE_FUNC(func, returnType, ...) MH_DECLARE_FUNC_LINE(__LINE__, func, returnType, __VA_ARGS__)
#define MH_DEFINE_FUNC(func, returnType, ...) returnType (*func)(__VA_ARGS__) = [](__VA_ARGS__) -> returnType
#else
#define MH_DECLARE_FUNC(func, returnType, ...) static returnType func(__VA_ARGS__);
#define MH_DEFINE_FUNC(func, returnType, ...) returnType func(__VA_ARGS__)
#endif

namespace Mahakam
{
	class SharedLibrary
	{
	public:
		static constexpr int NUM_FUNC_PTRS = 110;

		typedef void** FuncPtr;

	private:
		std::filesystem::path m_Filepath;
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