#include "mhpch.h"
#include "Light.h"

#include "FrameBuffer.h"

namespace Mahakam
{
	Light::Light(LightType lightType, const glm::vec3& color, bool shadowCasting)
		: lightType(lightType), color(color), shadowCasting(shadowCasting) {}

	Light::Light(LightType lightType, float range, const glm::vec3& color, bool shadowCasting)
		: lightType(lightType), range(range), color(color), shadowCasting(shadowCasting) {}

	Light::Light(LightType lightType, float fov, float range, const glm::vec3& color, bool shadowCasting)
		: lightType(lightType), fov(fov), range(range), color(color), shadowCasting(shadowCasting) {}
}