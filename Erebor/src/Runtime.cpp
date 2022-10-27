#include "ebpch.h"
#include "Runtime.h"

namespace Mahakam::Editor
{
	void Runtime::LoadRuntime(const std::filesystem::path& filepath, const std::string& filename)
	{
		// Unload previous runtime, if loaded
		if (s_Runtime)
			UnloadRuntime();

		s_Filepath = filepath;
		s_Filename = filename;

#if MH_PLATFORM_WINDOWS
		std::filesystem::path originalPath = filepath / (filename + ".dll");
		std::filesystem::path runtimePath = filepath / (filename + "-runtime.dll");
#else
		std::filesystem::path originalPath = filepath / ("lib" + filename + ".so");
		std::filesystem::path runtimePath = filepath / ("lib" + filename + "-runtime.so");
#endif

		// Copy the shared library
		std::filesystem::copy(originalPath, runtimePath, std::filesystem::copy_options::update_existing);

		s_Runtime = CreateScope<SharedLibrary>(runtimePath);

		s_Runtime->Load();

		s_RunPtr = s_Runtime->GetFunction<void, Scene*>("Run");
		s_StopPtr = s_Runtime->GetFunction<void, Scene*>("Stop");
		s_UpdatePtr = s_Runtime->GetFunction<void, Scene*, Timestep>("Update");
	}

	void Runtime::UnloadRuntime()
	{
		s_Runtime->Unload();

		s_Runtime = nullptr;

		s_RunPtr = nullptr;
		s_StopPtr = nullptr;
		s_UpdatePtr = nullptr;
	}

	bool Runtime::HasRuntime(bool& hasRun, bool& hasStop, bool& hasUpdate)
	{
		if (s_Runtime)
		{
			hasRun = s_RunPtr;
			hasStop = s_StopPtr;
			hasUpdate = s_UpdatePtr;

			return true;
		}

		hasRun = false;
		hasStop = false;
		hasUpdate = false;

		return false;
	}
}