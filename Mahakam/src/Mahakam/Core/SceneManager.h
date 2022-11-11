#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Scene/Scene.h"

namespace Mahakam
{
	class SceneManager
	{
	private:
		inline static Ref<Scene> s_ActiveScene;
#ifndef MH_STANDALONE
		inline static Ref<Scene> s_InactiveScene;
#endif

	public:
		inline static void SetActiveScene(Ref<Scene> scene) { s_ActiveScene = std::move(scene); }
		inline static Ref<Scene> GetActiveScene() { return s_ActiveScene; }

		inline static void MakeSceneInactive()
		{
			s_InactiveScene = s_ActiveScene;
		}
	};
}
