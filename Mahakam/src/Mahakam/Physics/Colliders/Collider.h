#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Types.h"

namespace Mahakam
{
	class PhysicsContext;
	class Rigidbody;

	class Collider
	{
	public:
		virtual ~Collider() = default;

		virtual void SetRigidbody(Rigidbody* rigidbody) = 0;
		virtual Rigidbody* GetRigidbody() = 0;

		//virtual bool Intersect(PhysicsContext* context) = 0;
	};
}