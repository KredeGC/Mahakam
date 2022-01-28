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

		Light& getLight() { return light; }
	};
}