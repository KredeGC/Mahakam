#include "mhpch.h"
#include "SharedLibrary.h"

namespace Mahakam
{
	SharedLibrary::SharedLibrary(const char* filepath) : filepath(filepath)
	{
#if defined(MH_PLATFORM_WINDOWS)
		size_t size = strlen(filepath) + 1;
		wchar_t* wa = new wchar_t[size];
		mbstowcs(wa, filepath, size);

		handle = LoadLibrary(wa);

		delete[] wa;
#elif defined(MH_PLATFORM_LINUX)
		handle = dlopen(filepath, RTLD_NOW);
#endif
	}

	SharedLibrary::~SharedLibrary()
	{
#if defined(MH_PLATFORM_WINDOWS)
		MH_ASSERT(FreeLibrary(handle), "Failed to close shared library!");
#elif defined(MH_PLATFORM_LINUX)
		MH_ASSERT(dlclose(handle), "Failed to close shared library!");
#endif
	}

	void* SharedLibrary::GetFunction(const char* name)
	{
#if defined(MH_PLATFORM_WINDOWS)
		return GetProcAddress(handle, name);
#elif defined(MH_PLATFORM_LINUX)
		return dlsym(myso, name);
#endif
	}
}