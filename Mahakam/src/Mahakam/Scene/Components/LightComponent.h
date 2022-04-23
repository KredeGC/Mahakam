#pragma once

#include "Mahakam/Renderer/Light.h"

namespace Mahakam
{
	struct LightComponent
	{
	private:
		Light light;

	public:
		LightComponent() = default;

		LightComponent(Light::LightType lightType, float range, const glm::vec3& color, bool shadowCasting = false) // Directional
			: light(lightType, range, color, shadowCasting) {}

		LightComponent(Light::LightType lightType, float range, const glm::vec3& color) // Point
			: light(lightType, range, color) {}

		LightComponent(Light::LightType lightType, float fov, float range, const glm::vec3& color, bool shadowCasting = false) // Spot
			: light(lightType, fov, range, color, shadowCasting) {}

		operator Light& () { return light; }
		operator const Light& () const { return light; }

		Light& GetLight() { return light; }
	};
}