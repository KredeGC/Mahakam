#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Mahakam
{
	class Light
	{
	public:
		enum class LightType
		{
			Directional = 0,
			Spot,
			Point
		};

	private:
		LightType lightType = LightType::Directional;
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };

		float range = 10.0f;
		float fov = glm::radians(60.0f);

	public:
		Light() = default;

		Light(LightType lightType, const glm::vec3& color)
			: lightType(lightType), color(color) {}

		Light(LightType lightType, float range, const glm::vec3& color)
			: lightType(lightType), range(range), color(color) {}

		Light(LightType lightType, float fov, float range, const glm::vec3& color)
			: lightType(lightType), fov(fov), range(range), color(color) {}

		inline void setLightType(LightType type) { lightType = type; }
		inline void setColor(const glm::vec3& col) { color = col; }
		inline void setFov(float fieldOfView) { fov = fieldOfView; }
		inline void setRange(float dist) { range = dist; }

		inline LightType getLightType() const { return lightType; }
		inline const glm::vec3& getColor() const { return color; }
		inline float getRange() const { return range; }
		inline float getFov() const { return fov; }
	};
}