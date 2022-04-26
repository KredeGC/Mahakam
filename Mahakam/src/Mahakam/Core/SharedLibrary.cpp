#include "mhpch.h"
#include "SharedLibrary.h"

#include <imgui.h>

namespace Mahakam
{
	SharedLibrary::SharedLibrary(const char* filepath) : filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

#if defined(MH_PLATFORM_WINDOWS)
		size_t size = strlen(filepath) + 1;
		wchar_t* wa = new wchar_t[size];
		mbstowcs(wa, filepath, size);

		handle = LoadLibrary(wa);

		delete[] wa;
#elif defined(MH_PLATFORM_LINUX)
		handle = dlopen(filepath, RTLD_NOW);
#endif

		MH_CORE_ASSERT(handle, "Failed to open shared library!");

		if (!initialized)
			ExportFuncPointers();

		auto loadPtr = GetFunction<void, ImGuiContext*, void**>("Load");

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

	void SharedLibrary::AddFunction(FuncPtr* funcPtr)
	{
		funcPointers[funcPointerCounter++] = funcPtr;
	}

	void SharedLibrary::ExportFuncPointers()
	{
		initialized = true;

		int i = 0;

		
	}

	void SharedLibrary::ImportFuncPointers(void* ptrs[NUM_FUNC_PTRS])
	{
		initialized = true;

		for (int i = 0; i < NUM_FUNC_PTRS; i++)
		{
			funcPointers[i] = ptrs[i];
		}
	}
}