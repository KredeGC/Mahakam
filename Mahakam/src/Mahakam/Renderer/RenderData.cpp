#include "mhpch.h"
#include "RenderData.h"

#include "Light.h"
#include "Camera.h"

namespace Mahakam
{
	DirectionalLight::DirectionalLight(const glm::vec3& direction, const Light& light) :
		direction(direction), color(light.GetColor()),
		worldToLight(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.03f, 20.0f)* glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f))) {}

	PointLight::PointLight(const glm::vec3& position, const Light& light) :
		position(glm::vec4(position, light.GetRange())),
		color(light.GetColor(), 1.0f / (light.GetRange() * light.GetRange())) {}

	SpotLight::SpotLight(const glm::vec3& position, const glm::quat& rotation, const Light& light) :
		worldToLight(glm::perspective(light.GetFov(), 1.0f, 0.03f, light.GetRange())* glm::inverse(glm::translate(glm::mat4(1.0f), position)* glm::mat4(rotation))),
		color(light.GetColor(), 1.0f / (light.GetRange() * light.GetRange()))
	{
		float xy = glm::tan(light.GetFov() / 2.0f) * light.GetRange();
		objectToWorld = glm::translate(glm::mat4(1.0f), position)
			* glm::mat4(rotation)
			* glm::scale(glm::mat4(1.0f), glm::vec3(xy, xy, light.GetRange()));
	}

	CameraData::CameraData(const Camera& camera, const glm::mat4& transform) :
		u_m4_V(glm::inverse(transform)),
		u_m4_P(camera.GetProjectionMatrix()),
		u_m4_IV(transform),
		u_m4_IP(glm::inverse(camera.GetProjectionMatrix())),
		u_m4_VP(u_m4_P* u_m4_V),
		u_m4_IVP(u_m4_IV* u_m4_IP),
		u_CameraPos(transform[3]) {}
}