#pragma once
#include "Core.h"

namespace Mahakam
{
	class SharedLibrary
	{
	private:
		const char* filepath = nullptr;
#if defined(MH_PLATFORM_WINDOWS)
		HINSTANCE handle;
#else
		void* handle = nullptr;
#endif

	public:
		SharedLibrary(const char* filepath);

		~SharedLibrary();

		void* GetFunction(const char* name);
	};
}