#include "Mahakam/mhpch.h"
#include "SharedLibrary.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

#include <imgui/imgui.h>

#if MH_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Mahakam
{
	static constexpr int NUM_FUNC_PTRS = 112;

	static bool g_SharedLibraryInitialized = false;
	static int g_FuncPointerCounter = 0;
	static SharedLibrary::SharedFunctor g_FuncPointers[NUM_FUNC_PTRS];

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
			MH_WARN("Failed to open shared library {0}", m_Filepath);

		if (!g_SharedLibraryInitialized)
			ExportFuncPointers();
        
        auto contextPtr = GetFunction<void, ImGuiContext*, spdlog::logger*, SharedFunctor*>("LoadContext");

		ImGuiContext* context = ImGui::GetCurrentContext();
        std::shared_ptr<spdlog::logger> logger = Log::GetLogger();

		if (contextPtr)
			contextPtr(context, logger.get(), g_FuncPointers);
	}

	SharedLibrary::~SharedLibrary()
	{
		MH_PROFILE_FUNCTION();
        
        auto contextPtr = GetFunction<void>("UnloadContext");

		if (contextPtr)
			contextPtr();

#if defined(MH_PLATFORM_WINDOWS)
		if (!FreeLibrary(reinterpret_cast<HINSTANCE>(m_Handle)))
			MH_WARN("Failed to close shared library {0}", m_Filepath);
#elif defined(MH_PLATFORM_LINUX)
		dlclose(m_Handle);
#endif
	}

	void SharedLibrary::Load()
	{
		MH_PROFILE_FUNCTION();

		auto loadPtr = GetFunction<void>("Load");

		if (loadPtr)
			loadPtr();
	}

	void SharedLibrary::Unload()
	{
		MH_PROFILE_FUNCTION();

		auto unloadPtr = GetFunction<void>("Unload");

		if (unloadPtr)
			unloadPtr();
	}

	void* SharedLibrary::GetFunction(const char* name)
	{
#if defined(MH_PLATFORM_WINDOWS)
		return GetProcAddress(reinterpret_cast<HINSTANCE>(m_Handle), name);
#elif defined(MH_PLATFORM_LINUX)
		return dlsym(m_Handle, name);
#endif
	}

	void SharedLibrary::AddExportFunction(FuncPtr funcPtr, LocPtr location)
	{
		g_FuncPointers[g_FuncPointerCounter].Func = funcPtr;
		g_FuncPointers[g_FuncPointerCounter++].Location = location;
	}

	void SharedLibrary::ExportFuncPointers()
	{
		g_SharedLibraryInitialized = true;

		MH_ASSERT(g_FuncPointerCounter == NUM_FUNC_PTRS, "Inconsisent amount of func pointers exported!");
	}

	void SharedLibrary::ImportFuncPointers(SharedFunctor* ptrs)
	{
		g_SharedLibraryInitialized = true;

		for (int i = 0; i < NUM_FUNC_PTRS; i++)
		{
			// Get location from array
			// Set our location based on the parameters
			g_FuncPointers[i](*ptrs[i].Location);
		}
	}
}