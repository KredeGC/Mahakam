#pragma once

#include "Mahakam/Physics/Colliders/SphereCollider.h"

#include <btBulletDynamicsCommon.h>

namespace Mahakam
{
	class BulletSphereCollider : public SphereCollider
	{
	private:
		btSphereShape m_Shape;
		float m_Radius;

	public:
		BulletSphereCollider(float radius);
		~BulletSphereCollider();

		virtual void SetRadius(float value) override;
		virtual float GetRadius() const override { return m_Radius; }
	};
}