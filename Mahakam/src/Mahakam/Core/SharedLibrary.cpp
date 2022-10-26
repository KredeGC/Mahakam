#include "Mahakam/mhpch.h"
#include "SharedLibrary.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	SharedLibrary::SharedLibrary(const std::filesystem::path& filepath) : m_Filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

		std::string pathString = m_Filepath.string();

#if defined(MH_PLATFORM_WINDOWS)
		m_Handle = LoadLibraryA(pathString.c_str());
#elif defined(MH_PLATFORM_LINUX)
		m_Handle = dlopen(pathString.c_str(), RTLD_NOW);
#endif

		if (!m_Handle)
			MH_CORE_WARN("Failed to open shared library {0}", m_Filepath);

		if (!s_Initialized)
			ExportFuncPointers();
	}

	SharedLibrary::~SharedLibrary()
	{
		MH_PROFILE_FUNCTION();

#if defined(MH_PLATFORM_WINDOWS)
		if (!FreeLibrary(m_Handle))
			MH_CORE_WARN("Failed to close shared library {0}", m_Filepath);
#elif defined(MH_PLATFORM_LINUX)
		dlclose(m_Handle);
#endif
	}

	void SharedLibrary::Load()
	{
		MH_PROFILE_FUNCTION();

		auto loadPtr = GetFunction<void, ImGuiContext*, FuncPtr*>("Load");

		ImGuiContext* context = ImGui::GetCurrentContext();

		if (loadPtr)
			loadPtr(context, s_FuncPointers);
	}

	void SharedLibrary::Unload()
	{
		MH_PROFILE_FUNCTION();

		auto unloadPtr = GetFunction<void>("Unload");

		if (unloadPtr)
			unloadPtr();
	}

	void SharedLibrary::AddExportFunction(FuncPtr funcPtr)
	{
		s_FuncPointers[s_FuncPointerCounter++] = funcPtr;
	}

	void SharedLibrary::ExportFuncPointers()
	{
		s_Initialized = true;

		MH_CORE_ASSERT(s_FuncPointerCounter == NUM_FUNC_PTRS, "Inconsisent amount of func pointers exported!");
	}

	void SharedLibrary::ImportFuncPointers(FuncPtr ptrs[NUM_FUNC_PTRS])
	{
		s_Initialized = true;

		for (int i = 0; i < NUM_FUNC_PTRS; i++)
		{
			if (s_FuncPointers[i] && *s_FuncPointers[i])
				*s_FuncPointers[i] = *ptrs[i];
		}
	}
}