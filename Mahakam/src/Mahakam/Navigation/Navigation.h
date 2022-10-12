#pragma once

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>

namespace Mahakam
{
	class Navigation
	{
	public:
		void AddNode(const glm::vec3& pos);

		bool FindPath(const glm::vec3& start, const glm::vec3& end);
	};
}