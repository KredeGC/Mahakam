#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Scene/Scene.h"

namespace Mahakam::Editor
{
    class SceneManager
    {
    private:
		inline static Ref<Scene> s_ActiveScene;

    public:
		inline static void SetActiveScene(Ref<Scene> scene) { s_ActiveScene = scene; }
		inline static Ref<Scene> GetActiveScene() { return s_ActiveScene; }
    };
}
