#pragma once
#include "Core.h"

namespace Mahakam
{
	class SharedLibrary
	{
	private:
		const char* filepath = nullptr;
#if defined(MH_PLATFORM_WINDOWS)
		HINSTANCE handle = 0;
#else
		void* handle = nullptr;
#endif

	public:
		SharedLibrary() = default;

		SharedLibrary(const char* filepath);

		~SharedLibrary();

		template<typename R, typename ...Args>
		auto GetFunction(const char* name)
		{
#if defined(MH_PLATFORM_WINDOWS)
			return (R (*)(Args...))GetProcAddress(handle, name);
#elif defined(MH_PLATFORM_LINUX)
			return (R (*)(Args...))dlsym(myso, name);
#endif
		}
	};
}