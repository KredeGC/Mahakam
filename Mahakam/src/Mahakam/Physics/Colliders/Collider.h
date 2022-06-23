#pragma once

namespace Mahakam
{
	class PhysicsContext;

	class Collider
	{
	public:
		virtual ~Collider() = default;

		virtual bool Intersect(PhysicsContext* context) = 0;
	};
}