#pragma once
#include "Mahakam/Core/Core.h"

#if MH_PLATFORM_LINUX
#include <dlfcn.h>
#endif

namespace Mahakam
{
	class SharedLibrary
	{
	public:
		typedef void* FuncPtr;

		static constexpr int NUM_FUNC_PTRS = 3;

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

		static void AddFunction(void** funcPtr);

		static void ExportFuncPointers();
		static void ImportFuncPointers(void* ptrs[NUM_FUNC_PTRS]);

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