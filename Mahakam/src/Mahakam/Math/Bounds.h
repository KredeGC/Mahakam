#pragma once

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

		Bounds(const glm::vec3& min, const glm::vec3& max)
			: Min(min), Max(max)
		{
			Positions[0] = min;
			Positions[1] = { min.x, max.y, min.z };
			Positions[2] = { min.x, min.y, max.z };
			Positions[3] = { min.x, max.y, max.z };

			Positions[4] = { max.x, min.y, min.z };
			Positions[5] = { max.x, max.y, min.z };
			Positions[6] = { max.x, min.y, max.z };
			Positions[7] = max;
		}

		static Bounds CalculateBounds(const glm::vec3* positions, uint32_t vertexCount);
		static Bounds TransformBounds(const Bounds& bounds, const glm::mat4& transform);
	};
}