#pragma once

#include <glm/glm.hpp>

namespace Mahakam
{
	class FrameBuffer;

	class Light
	{
	public:
		enum class LightType
		{
			Directional = 0,
			Point,
			Spot
		};

	private:
		LightType lightType = LightType::Directional;
		glm::vec3 color = { 1.0f, 1.0f, 1.0f };

		float range = 10.0f;
		float fov = glm::radians(60.0f);
		float shadowBias = 0.005f;

		bool shadowCasting = false;

	public:
		Light() = default;

		Light(LightType lightType, float range, const glm::vec3& color, bool shadowCasting = false, float bias = 0.005f); // Directional

		Light(LightType lightType, float fov, float range, const glm::vec3& color, bool shadowCasting = false, float bias = 0.0f); // Spot

		inline void SetLightType(LightType type) { lightType = type; }
		inline void SetColor(const glm::vec3& col) { color = col; }
		inline void SetRange(float dist) { range = dist; }
		inline void SetFov(float fieldOfView) { fov = fieldOfView; }
		inline void SetBias(float bias) { shadowBias = bias; }
		inline void SetShadowCasting(bool shadows) { shadowCasting = shadows; }

		inline LightType GetLightType() const { return lightType; }
		inline const glm::vec3& GetColor() const { return color; }
		inline float GetRange() const { return range; }
		inline float GetFov() const { return fov; }
		inline float GetBias() const { return shadowBias; }
		inline bool IsShadowCasting() const { return shadowCasting; }
	};
}