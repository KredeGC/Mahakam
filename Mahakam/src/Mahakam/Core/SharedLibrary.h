#pragma once

#include "Mahakam/Core/Core.h"

#if MH_PLATFORM_WINDOWS
#ifndef _WINDEF_
struct HINSTANCE__; // Forward or never
typedef HINSTANCE__* HINSTANCE;
#endif
#elif MH_PLATFORM_LINUX
#include <dlfcn.h>
#endif

namespace Mahakam
{
	class SharedLibrary
	{
	public:
		static constexpr int NUM_FUNC_PTRS = 101;

	private:
		const char* filepath = nullptr;
#if defined(MH_PLATFORM_WINDOWS)
		HINSTANCE handle = 0;
#elif defined(MH_PLATFORM_LINUX)
		void* handle = nullptr;
#endif

		inline static bool initialized = false;
		inline static FuncPtr funcPointers[NUM_FUNC_PTRS];
		inline static int funcPointerCounter = 0;

	public:
		SharedLibrary() = default;

		SharedLibrary(const char* filepath);

		virtual ~SharedLibrary();

		static void AddFunction(FuncPtr funcPtr);

		static void ExportFuncPointers();
		static void ImportFuncPointers(FuncPtr ptrs[NUM_FUNC_PTRS]);

		template<typename R, typename ...Args>
		auto GetFunction(const char* name)
		{
#if defined(MH_PLATFORM_WINDOWS)
			return (R (*)(Args...))GetProcAddress(handle, name);
#elif defined(MH_PLATFORM_LINUX)
			return (R (*)(Args...))dlsym(handle, name);
#endif
		}
	};
}