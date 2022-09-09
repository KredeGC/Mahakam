#include "Mahakam/mhpch.h"
#include "Light.h"

namespace Mahakam
{
	Light::Light(LightType lightType, float range, const glm::vec3& color, bool shadowCasting, float bias)
		: m_LightType(lightType), m_Range(range), m_Color(color), m_ShadowCasting(shadowCasting), m_ShadowBias(bias) {}

	Light::Light(LightType lightType, float fov, float range, const glm::vec3& color, bool shadowCasting, float bias)
		: m_LightType(lightType), m_Fov(fov), m_Range(range), m_Color(color), m_ShadowCasting(shadowCasting), m_ShadowBias(bias) {}
}