#pragma once

#include "Mahakam/Physics/Colliders/Collider.h"

#include <btBulletDynamicsCommon.h>

namespace Mahakam
{
	class BulletCollider : public Collider
	{
	public:
		virtual ~BulletCollider() = default;

		virtual btCollisionShape* GetShape() = 0;
	};
}