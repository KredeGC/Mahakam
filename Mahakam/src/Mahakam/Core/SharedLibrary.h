#pragma once

#include "Core.h"

#include <filesystem>

#if MH_PLATFORM_LINUX
#include <dlfcn.h>
#endif

// Function declarations which should be reachable in DLLs
#ifdef MH_ENABLE_DYNAMIC_LINKING
#define MH_DECLARE_FUNC_LINE2(line, func, returnType, ...) static returnType (*func)(__VA_ARGS__); \
	struct generated_struct_##func##_##line { generated_struct_##func##_##line() { ::Mahakam::SharedLibrary::AddExportFunction(&func); } }; \
	inline static generated_struct_##func##_##line generated_var_##func##_##line;

#define MH_DECLARE_FUNC_LINE(line, func, returnType, ...) MH_DECLARE_FUNC_LINE2(line, func, returnType, __VA_ARGS__)

#define MH_DECLARE_FUNC(func, returnType, ...) MH_DECLARE_FUNC_LINE(__LINE__, func, returnType, __VA_ARGS__)
#define MH_DEFINE_FUNC(func, returnType, ...) returnType (*func)(__VA_ARGS__) = [](__VA_ARGS__) -> returnType
#else // MH_ENABLE_DYNAMIC_LINKING
#define MH_DECLARE_FUNC(func, returnType, ...) static returnType func(__VA_ARGS__);
#define MH_DEFINE_FUNC(func, returnType, ...) returnType func(__VA_ARGS__)
#endif // MH_ENABLE_DYNAMIC_LINKING

namespace Mahakam
{
	class SharedLibrary
	{
	public:
		using StatePtr = void(*)();
		using LocPtr = StatePtr*;
		using FuncPtr = void(*)(LocPtr, StatePtr);

		struct SharedFunctor
		{
			LocPtr Location;
			FuncPtr Func;

			void operator()(StatePtr ptr)
			{
				Func(Location, ptr);
			}
		};

	private:
		std::filesystem::path m_Filepath;
		void* m_Handle = nullptr;

	public:
		SharedLibrary() = default;

		SharedLibrary(const std::filesystem::path& filepath);

		~SharedLibrary();

		void Load();
		void Unload();
		void* GetFunction(const char* name);

		template<typename R, typename ...Args>
		auto GetFunction(const char* name)
		{
			return reinterpret_cast<R(*)(Args...)>(GetFunction(name));
		}

		static void AddExportFunction(FuncPtr funcPtr, LocPtr location);

		template<typename T>
		static void AddExportFunction(T* funcPtr)
		{
			AddExportFunction([](LocPtr location, StatePtr ptr) { *reinterpret_cast<T*>(location) = reinterpret_cast<T>(ptr); }, reinterpret_cast<LocPtr>(funcPtr));
		}

		static void ExportFuncPointers();
		static void ImportFuncPointers(SharedFunctor* ptrs);
	};
}