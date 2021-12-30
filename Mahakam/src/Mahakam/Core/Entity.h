#pragma once

#include "Core.h"
#include "Transform.h"

namespace Mahakam
{
	class Entity
	{
	private:
		Transform transform;

	public:
		virtual ~Entity() = default;

		inline const Transform& getTransform() const { return transform; }
	};
}