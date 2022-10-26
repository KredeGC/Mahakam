#pragma once

#include <Mahakam/Mahakam.h>

#include <filesystem>

namespace Mahakam::Editor
{
	class Runtime
	{
	private:
		inline static Scope<SharedLibrary> s_Runtime;

		inline static std::filesystem::path s_Filepath;
		inline static std::string s_Filename;

		inline static void (*s_RunPtr)(Scene*) = nullptr;
		inline static void (*s_StopPtr)(Scene*) = nullptr;
		inline static void (*s_UpdatePtr)(Scene*, Timestep) = nullptr;

	public:
		inline static void ReloadRuntime()
		{
			LoadRuntime(s_Filepath, s_Filename);
		}

		inline static void LoadRuntime(const std::filesystem::path& filepath, const std::string& filename)
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

		inline static void UnloadRuntime()
		{
			s_Runtime->Unload();

			s_Runtime = nullptr;

			s_RunPtr = nullptr;
			s_StopPtr = nullptr;
			s_UpdatePtr = nullptr;
		}

		inline static bool HasRuntime(bool& hasRun, bool& hasStop, bool& hasUpdate)
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

		inline static void RunScene(Ref<Scene> scene)
		{
			s_RunPtr(scene.get());
		}

		inline static void StopScene(Ref<Scene> scene)
		{
			s_StopPtr(scene.get());
		}

		inline static void UpdateScene(Ref<Scene> scene, Timestep dt)
		{
			if (s_UpdatePtr)
				s_UpdatePtr(scene.get(), dt);
		}
	};
}