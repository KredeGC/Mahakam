#include "mhpch.h"
#include "Light.h"

#include "FrameBuffer.h"

namespace Mahakam
{
	Light::Light(LightType lightType, float range, const glm::vec3& color, bool shadowCasting, float bias)
		: lightType(lightType), range(range), color(color), shadowCasting(shadowCasting), shadowBias(bias) {}

	Light::Light(LightType lightType, float fov, float range, const glm::vec3& color, bool shadowCasting, float bias)
		: lightType(lightType), fov(fov), range(range), color(color), shadowCasting(shadowCasting), shadowBias(bias) {}
}