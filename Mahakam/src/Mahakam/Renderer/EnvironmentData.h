#pragma once

#include "Mahakam/Core/Types.h"

#include "Mahakam/Asset/Asset.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace Mahakam
{
	// Forward declare most things, don't want to pollute the header files too much
	class Light;
	class Material;
	class TextureCube;

	struct DirectionalLight
	{
		glm::vec3 direction;
		float padding01 = 0.0f;
		glm::vec3 color;
		float padding02 = 0.0f;
		glm::mat4 worldToLight;
		glm::vec4 volumetric = { 1.0f, 1.0f, 1.0f, 0.95f }; // xyz - color, w - scattering
		glm::vec4 offset = { 0.0f, 0.0f, 0.0f, 0.0f }; // xy - offset in shadow map, z - scale in shadow map, w - bias

		DirectionalLight() = default;
		DirectionalLight(const DirectionalLight&) = default;
		DirectionalLight(DirectionalLight&&) = default;
		DirectionalLight(const glm::vec3& position, const glm::quat& rotation, const Light& light);

		DirectionalLight& operator=(const DirectionalLight&) = default;

		uint64_t Hash();
	};

	struct PointLight
	{
		glm::vec4 position; // w - range
		glm::vec4 color; // w - 1.0 / (range * range)

		PointLight() = default;
		PointLight(const PointLight&) = default;
		PointLight(PointLight&&) = default;
		PointLight(const glm::vec3& position, const Light& light);

		PointLight& operator=(const PointLight&) = default;

		uint64_t Hash();
	};

	struct SpotLight
	{
		glm::mat4 objectToWorld;
		glm::vec4 color; // w - 1.0 / (range * range)
		glm::mat4 worldToLight;
		glm::vec4 volumetric = { 1.0f, 1.0f, 1.0f, 0.95f }; // xyz - color, w - scattering
		glm::vec4 offset = { 0.0f, 0.0f, 0.0f, 0.0f }; // xy - offset in shadow map, z - scale in shadow map, w - bias

		SpotLight() = default;
		SpotLight(const SpotLight&) = default;
		SpotLight(SpotLight&&) = default;
		SpotLight(const glm::vec3& position, const glm::quat& rotation, const Light& light);

		SpotLight& operator=(const SpotLight&) = default;

		uint64_t Hash();
	};

	struct EnvironmentData
	{
		Asset<Material> SkyboxMaterial;
		Asset<TextureCube> IrradianceMap;
		Asset<TextureCube> SpecularMap;
		TrivialVector<DirectionalLight> DirectionalLights;
		TrivialVector<PointLight> PointLights;
		TrivialVector<SpotLight> SpotLights;
	};
}