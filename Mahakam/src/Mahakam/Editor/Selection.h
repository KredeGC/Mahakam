#pragma once

#include "Mahakam/Scene/Entity.h"

#include <glm/glm.hpp>

namespace Mahakam::Editor
{
    class Selection
    {
    private:
        inline static glm::vec3 s_OrbitTarget{ 0.0f, 0.0f, 0.0f };
        inline static Entity s_SelectedEntity;

    public:
        inline static void SetOrbitTarget(const glm::vec3& target) { s_OrbitTarget = target; }
        inline static const glm::vec3& GetOrbitTarget() { return s_OrbitTarget; }

		inline static void SetSelectedEntity(Entity entity) { s_SelectedEntity = entity; }
		inline static Entity GetSelectedEntity() { return s_SelectedEntity; }
    };
}