#pragma once

#include "Collider.h"

#include <glm/ext/vector_float3.hpp>

namespace Mahakam
{
	class BoxCollider : public Collider
	{
	public:
		virtual ~BoxCollider() = default;

		virtual void SetSize(const glm::vec3& extents) = 0;
		virtual const glm::vec3& GetSize() const = 0;

		inline static Ref<BoxCollider> Create() { return Create({ 1.0f, 1.0f, 1.0f }); }
		static Ref<BoxCollider> Create(const glm::vec3& extents);
	};
}