#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Mahakam
{
	class FrameBuffer;

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

		bool shadowCasting = false;
		Ref<FrameBuffer> shadowFramebuffer = nullptr;

	public:
		Light() = default;

		Light(LightType lightType, const glm::vec3& color);

		Light(LightType lightType, float range, const glm::vec3& color);

		Light(LightType lightType, float fov, float range, const glm::vec3& color);

		inline void SetLightType(LightType type) { lightType = type; }
		inline void SetColor(const glm::vec3& col) { color = col; }
		inline void SetFov(float fieldOfView) { fov = fieldOfView; }
		inline void SetRange(float dist) { range = dist; }

		inline LightType GetLightType() const { return lightType; }
		inline const glm::vec3& GetColor() const { return color; }
		inline float GetRange() const { return range; }
		inline float GetFov() const { return fov; }
	};
}