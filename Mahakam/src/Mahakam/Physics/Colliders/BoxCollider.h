#pragma once

#include "Collider.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	class BoxCollider : public Collider
	{
	public:
		virtual ~BoxCollider() = default;

		virtual void SetSize(const glm::vec3& extents) = 0;
		virtual const glm::vec3& GetSize() const = 0;
	};
}