#include "Mahakam/mhpch.h"
#include "BulletRigidbody.h"

#include "BulletPhysicsContext.h"

#include <btBulletDynamicsCommon.h>

namespace Mahakam
{
	BulletRigidbody::BulletRigidbody(const glm::vec3& position, float mass, PhysicsContext* context)
	{
		BulletPhysicsContext* bulletContext = static_cast<BulletPhysicsContext*>(context);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin({ position.x, position.y, position.z });

		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		/*if (isDynamic)
			shape->calculateLocalInertia(mass, localInertia);*/

		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, nullptr, localInertia);

		m_Rigidbody = new btRigidBody(rbInfo);

		bulletContext->GetDynamicsWorld()->addRigidBody(m_Rigidbody);
	}

	BulletRigidbody::~BulletRigidbody()
	{
		delete m_Rigidbody;
	}

	void BulletRigidbody::SetMass(float mass)
	{
		// TODO
	}

	float BulletRigidbody::GetMass() const
	{
		// TODO
		return 0.0f;
	}

	void BulletRigidbody::SetVelocity(const glm::vec3& velocity)
	{
		// TODO
	}
}