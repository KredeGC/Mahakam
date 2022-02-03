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

		LightComponent(Light::LightType lightType, const glm::vec3& color)
			: light(lightType, color) {}

		LightComponent(Light::LightType lightType, float range, const glm::vec3& color)
			: light(lightType, range, color) {}

		LightComponent(Light::LightType lightType, float fov, float range, const glm::vec3& color)
			: light(lightType, fov, range, color) {}

		operator const Light& () const { return light; }

		Light& GetLight() { return light; }
	};
}