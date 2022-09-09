#pragma once

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/trigonometric.hpp>

namespace Mahakam
{
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
		LightType m_LightType = LightType::Directional;
		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };

		float m_Range = 10.0f;
		float m_Fov = glm::radians(60.0f);
		float m_ShadowBias = 0.005f;

		bool m_ShadowCasting = false;

	public:
		Light() = default;

		Light(LightType lightType, float range, const glm::vec3& color, bool shadowCasting = false, float bias = 0.005f); // Directional

		Light(LightType lightType, float fov, float range, const glm::vec3& color, bool shadowCasting = false, float bias = 0.0f); // Spot

		inline void SetLightType(LightType type) { m_LightType = type; }
		inline void SetColor(const glm::vec3& col) { m_Color = col; }
		inline void SetRange(float dist) { m_Range = dist; }
		inline void SetFov(float fieldOfView) { m_Fov = fieldOfView; }
		inline void SetBias(float bias) { m_ShadowBias = bias; }
		inline void SetShadowCasting(bool shadows) { m_ShadowCasting = shadows; }

		inline LightType GetLightType() const { return m_LightType; }
		inline const glm::vec3& GetColor() const { return m_Color; }
		inline float GetRange() const { return m_Range; }
		inline float GetFov() const { return m_Fov; }
		inline float GetBias() const { return m_ShadowBias; }
		inline bool IsShadowCasting() const { return m_ShadowCasting; }
	};
}