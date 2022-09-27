#include "Mahakam/mhpch.h"
#include "Bounds.h"

namespace Mahakam
{
	Bounds::Bounds(const glm::vec3& min, const glm::vec3& max)
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

	Bounds Bounds::CalculateBounds(const glm::vec3* positions, uint32_t vertexCount)
	{
		glm::vec3 min = positions[0];
		glm::vec3 max = positions[0];

		for (uint32_t i = 1; i < vertexCount; i++)
		{
			const glm::vec3& pos = positions[i];
			if (pos.x < min.x)
				min.x = pos.x;
			if (pos.y < min.y)
				min.y = pos.y;
			if (pos.z < min.z)
				min.z = pos.z;

			if (pos.x > max.x)
				max.x = pos.x;
			if (pos.y > max.y)
				max.y = pos.y;
			if (pos.z > max.z)
				max.z = pos.z;
		}

		return { min, max };
	}

	Bounds Bounds::TransformBounds(const Bounds& bounds, const glm::mat4& transform)
	{
		glm::vec3 positions[8] = {
			glm::vec3{ transform * glm::vec4{ bounds.Positions[0], 1.0f } },
			glm::vec3{ transform * glm::vec4{ bounds.Positions[1], 1.0f } },
			glm::vec3{ transform * glm::vec4{ bounds.Positions[2], 1.0f } },
			glm::vec3{ transform * glm::vec4{ bounds.Positions[3], 1.0f } },
			glm::vec3{ transform * glm::vec4{ bounds.Positions[4], 1.0f } },
			glm::vec3{ transform * glm::vec4{ bounds.Positions[5], 1.0f } },
			glm::vec3{ transform * glm::vec4{ bounds.Positions[6], 1.0f } },
			glm::vec3{ transform * glm::vec4{ bounds.Positions[7], 1.0f } }
		};

		return CalculateBounds(positions, 8);
	}
}