#pragma once

#include "Collider.h"
#include "Mahakam/Core/Types.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>

namespace Mahakam
{
	class BoxCollider : public Collider
	{
	public:
		virtual ~BoxCollider() = default;

		virtual void SetSize(const glm::vec3& extents) = 0;
		virtual const glm::vec3& GetSize() const = 0;

		inline static Scope<BoxCollider> Create() { return Create({ 1.0f, 1.0f, 1.0f }); }
		static Scope<BoxCollider> Create(const glm::vec3& extents);
	};
}