#include "Mahakam/mhpch.h"
#include "SharedLibrary.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	SharedLibrary::SharedLibrary(const char* filepath) : filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

#if defined(MH_PLATFORM_WINDOWS)
		handle = LoadLibraryA(filepath);
#elif defined(MH_PLATFORM_LINUX)
		handle = dlopen(filepath, RTLD_NOW);
#endif

		MH_CORE_ASSERT(handle, "Failed to open shared library!");

		if (!initialized)
			ExportFuncPointers();

		auto loadPtr = GetFunction<void, ImGuiContext*, FuncPtr*>("Load");

		ImGuiContext* context = ImGui::GetCurrentContext();

		if (loadPtr)
			loadPtr(context, funcPointers);
	}

	SharedLibrary::~SharedLibrary()
	{
		MH_PROFILE_FUNCTION();

		auto unloadPtr = GetFunction<void>("Unload");

		if (unloadPtr)
			unloadPtr();

#if defined(MH_PLATFORM_WINDOWS)
		MH_CORE_ASSERT(FreeLibrary(handle), "Failed to close shared library!");
#elif defined(MH_PLATFORM_LINUX)
		dlclose(handle);
#endif
	}

	void SharedLibrary::AddFunction(FuncPtr funcPtr)
	{
		funcPointers[funcPointerCounter++] = funcPtr;
	}

	void SharedLibrary::ExportFuncPointers()
	{
		initialized = true;

		MH_CORE_ASSERT(funcPointerCounter == NUM_FUNC_PTRS, "Inconsisent amount of func pointers exported!");
	}

	void SharedLibrary::ImportFuncPointers(FuncPtr ptrs[NUM_FUNC_PTRS])
	{
		initialized = true;

		for (int i = 0; i < NUM_FUNC_PTRS; i++)
		{
			if (funcPointers[i] && *funcPointers[i])
				*funcPointers[i] = *ptrs[i];
		}
	}
}