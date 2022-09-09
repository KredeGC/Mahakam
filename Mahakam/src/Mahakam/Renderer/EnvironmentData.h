#pragma once

#include "Mahakam/Core/Core.h"

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

	extern template class Asset<Material>;
	extern template class Asset<TextureCube>;

	struct DirectionalLight
	{
	public:
		glm::vec3 direction;
	private:
		float padding01 = 0.0f;
	public:
		glm::vec3 color;
	private:
		float padding02 = 0.0f;
	public:
		glm::mat4 worldToLight;
		glm::vec4 offset = { 0.0f, 0.0f, 0.0f, 0.0f }; // xy - offset, z - size, w - bias

		DirectionalLight(const glm::vec3& position, const glm::quat& rotation, const Light& light);

		uint64_t Hash();
	};

	struct PointLight
	{
		glm::vec4 position; // w - range
		glm::vec4 color; // w - 1.0 / (range * range)

		PointLight(const glm::vec3& position, const Light& light);

		uint64_t Hash();
	};

	struct SpotLight
	{
		glm::mat4 objectToWorld;
		glm::vec4 color; // w - 1.0 / (range * range)
		glm::mat4 worldToLight;
		glm::vec4 offset = { 0.0f, 0.0f, 0.0f, 0.0f }; // xy - offset, z - size, w - bias

		SpotLight(const glm::vec3& position, const glm::quat& rotation, const Light& light);

		uint64_t Hash();
	};

	struct EnvironmentData
	{
		Asset<Material> SkyboxMaterial;
		Asset<TextureCube> IrradianceMap;
		Asset<TextureCube> SpecularMap;
		std::vector<DirectionalLight> DirectionalLights;
		std::vector<PointLight> PointLights;
		std::vector<SpotLight> SpotLights;
	};
}