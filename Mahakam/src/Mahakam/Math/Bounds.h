#pragma once

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace Mahakam
{
	class Bounds
	{
	public:
		glm::vec3 Min;
		glm::vec3 Max;

		glm::vec3 Positions[8];

		Bounds() = default;

		Bounds(const glm::vec3& min, const glm::vec3& max);

		static Bounds CalculateBounds(const glm::vec3* positions, uint32_t vertexCount);
		static Bounds TransformBounds(const Bounds& bounds, const glm::mat4& transform);
	};
}