#pragma once

#include "Collider.h"

namespace Mahakam
{
	class SphereCollider : public Collider
	{
	public:
		virtual ~SphereCollider() = default;

		virtual void SetRadius(float value) = 0;
		virtual float GetRadius() const = 0;
	};
}