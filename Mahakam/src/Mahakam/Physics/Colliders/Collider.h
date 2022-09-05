#pragma once

#include "Mahakam/Core/Core.h"

namespace Mahakam
{
	class PhysicsContext;
	class Rigidbody;

	class Collider
	{
	public:
		virtual ~Collider() = default;

		virtual void SetRigidbody(Ref<Rigidbody> rigidbody) = 0;
		virtual Ref<Rigidbody> GetRigidbody() = 0;

		//virtual bool Intersect(PhysicsContext* context) = 0;
	};
}