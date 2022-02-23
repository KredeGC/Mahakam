#include "mhpch.h"
#include "RenderData.h"

#include "Light.h"
#include "Camera.h"

namespace Mahakam
{
	DirectionalLight::DirectionalLight(const glm::vec3& position, const glm::quat& rotation, const Light& light) :
		direction(rotation * glm::vec3(0.0f, 0.0f, -1.0f)),
		color(light.GetColor())
	{
		worldToLight = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f)
			* glm::inverse(glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation));

		offset.z = light.IsShadowCasting();
		offset.w = light.GetBias();
	}

	uint64_t DirectionalLight::Hash()
	{
		const unsigned char* p = reinterpret_cast<const unsigned char*>(this);
		uint64_t h = 2166136261ULL;

		for (uint64_t i = 0; i < sizeof(DirectionalLight); ++i)
			h = (h * 16777619ULL) ^ p[i];

		return h;
	}


	PointLight::PointLight(const glm::vec3& position, const Light& light) :
		position(glm::vec4(position, light.GetRange())),
		color(light.GetColor(), 1.0f / (light.GetRange() * light.GetRange())) {}

	uint64_t PointLight::Hash()
	{
		const unsigned char* p = reinterpret_cast<const unsigned char*>(this);
		uint64_t h = 2166136261ULL;

		for (uint64_t i = 0; i < sizeof(PointLight); ++i)
			h = (h * 16777619ULL) ^ p[i];

		return h;
	}


	SpotLight::SpotLight(const glm::vec3& position, const glm::quat& rotation, const Light& light) :
		worldToLight(glm::perspective(light.GetFov(), 1.0f, 0.001f, light.GetRange()) * glm::inverse(glm::translate(glm::mat4(1.0f), position) * glm::mat4(rotation))),
		color(light.GetColor(), 1.0f / (light.GetRange() * light.GetRange()))
	{
		float xy = glm::tan(light.GetFov() / 2.0f) * light.GetRange();
		objectToWorld = glm::translate(glm::mat4(1.0f), position)
			* glm::mat4(rotation)
			* glm::scale(glm::mat4(1.0f), glm::vec3(xy, xy, light.GetRange()));

		offset.z = light.IsShadowCasting();
		offset.w = light.GetBias();
	}

	uint64_t SpotLight::Hash()
	{
		const unsigned char* p = reinterpret_cast<const unsigned char*>(this);
		uint64_t h = 2166136261ULL;

		for (uint64_t i = 0; i < sizeof(SpotLight); ++i)
			h = (h * 16777619ULL) ^ p[i];

		return h;
	}

	CameraData::CameraData(const Camera& camera, const glm::vec2& screenSize, const glm::mat4& transform) :
		u_m4_V(glm::inverse(transform)),
		u_m4_P(camera.GetProjectionMatrix()),
		u_m4_IV(transform),
		u_m4_IP(glm::inverse(camera.GetProjectionMatrix())),
		u_m4_VP(u_m4_P * u_m4_V),
		u_m4_IVP(u_m4_IV * u_m4_IP),
		u_CameraPos(transform[3]),
		u_ScreenParams(screenSize.x, screenSize.y, 1.0f / screenSize.x, 1.0f / screenSize.y) {}

	CameraData::CameraData(const glm::mat4& projectionMatrix, const glm::vec2& screenSize, const glm::mat4& transform) :
		u_m4_V(glm::inverse(transform)),
		u_m4_P(projectionMatrix),
		u_m4_IV(transform),
		u_m4_IP(glm::inverse(projectionMatrix)),
		u_m4_VP(u_m4_P* u_m4_V),
		u_m4_IVP(u_m4_IV* u_m4_IP),
		u_CameraPos(transform[3]),
		u_ScreenParams(screenSize.x, screenSize.y, 1.0f / screenSize.x, 1.0f / screenSize.y) {}
}