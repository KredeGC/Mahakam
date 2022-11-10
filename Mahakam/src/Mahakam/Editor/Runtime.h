#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Timestep.h"

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Scene;
	class SharedLibrary;
}

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
		static void LoadRuntime(const std::filesystem::path& filepath, const std::string& filename);

		static void UnloadRuntime();

		static bool HasRuntime(bool& hasRun, bool& hasStop, bool& hasUpdate);

		inline static void ReloadRuntime()
		{
			LoadRuntime(s_Filepath, s_Filename);
		}

		inline static void RunScene(const Ref<Scene>& scene)
		{
			if (s_RunPtr)
				s_RunPtr(scene.get());
		}

		inline static void StopScene(const Ref<Scene>& scene)
		{
			if (s_StopPtr)
				s_StopPtr(scene.get());
		}

		inline static void UpdateScene(const Ref<Scene>& scene, Timestep dt)
		{
			if (s_UpdatePtr)
				s_UpdatePtr(scene.get(), dt);
		}
	};
}