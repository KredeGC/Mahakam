#include "Mahakam/mhpch.h"
#include "EnvironmentData.h"

#include "Light.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Mahakam
{
	DirectionalLight::DirectionalLight(const glm::vec3& position, const glm::quat& rotation, const Light& light) :
		direction(rotation* glm::vec3(0.0f, 0.0f, -1.0f)),
		color(light.GetColor()),
		volumetric({ light.GetVolumetricColor(), light.GetVolumetricScattering() })
	{
		worldToLight = glm::ortho(-light.GetRange(), light.GetRange(), -light.GetRange(), light.GetRange(), -light.GetRange(), light.GetRange())
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
		worldToLight(glm::perspective(light.GetFov(), 1.0f, 0.001f, light.GetRange())* glm::inverse(glm::translate(glm::mat4(1.0f), position)* glm::mat4(rotation))),
		color(light.GetColor(), 1.0f / (light.GetRange() * light.GetRange())),
		volumetric({ light.GetVolumetricColor(), light.GetVolumetricScattering() })
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
}