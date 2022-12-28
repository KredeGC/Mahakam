#pragma once

#include "Mahakam/Renderer/Light.h"

namespace Mahakam
{
	struct LightComponent
	{
	private:
		Light m_Light;

	public:
		LightComponent() = default;

		LightComponent(Light::LightType lightType, float range, const glm::vec3& color, bool shadowCasting = false) // Directional
			: m_Light(lightType, range, color, shadowCasting) {}

		LightComponent(Light::LightType lightType, float range, const glm::vec3& color) // Point
			: m_Light(lightType, range, color) {}

		LightComponent(Light::LightType lightType, float fov, float range, const glm::vec3& color, bool shadowCasting = false) // Spot
			: m_Light(lightType, fov, range, color, shadowCasting) {}

		inline operator Light& () { return m_Light; }
		inline operator const Light& () const { return m_Light; }

		inline Light& GetLight() { return m_Light; }
	};
}