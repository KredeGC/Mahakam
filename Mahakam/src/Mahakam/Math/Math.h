#pragma once

#include <glm/glm.hpp>

namespace Mahakam::Math
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	bool DecomposeTransformRotation(const glm::mat4& transform, glm::quat& rotation);
}