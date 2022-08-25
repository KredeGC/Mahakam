#include "Mahakam/mhpch.h"
#include "BulletRigidbody.h"
#include "BulletPhysicsContext.h"
#include "Colliders/BulletCollider.h"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>

namespace Mahakam
{
	Ref<Rigidbody> Rigidbody::CreateImpl(PhysicsContext* context)
	{
		return CreateRef<BulletRigidbody>(context);
	}

	BulletRigidbody::BulletRigidbody(PhysicsContext* context)
	{
		glm::vec3 position{ 0.0f };
		float mass = 0.0f;

		m_PhysicsContext = static_cast<BulletPhysicsContext*>(context);

		m_Shape = new btCompoundShape();

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin({ position.x, position.y, position.z });

		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			m_Shape->calculateLocalInertia(mass, localInertia);

		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, m_Shape, localInertia);

		m_Rigidbody = new btRigidBody(rbInfo);

		m_PhysicsContext->GetDynamicsWorld()->addRigidBody(m_Rigidbody);
	}

	BulletRigidbody::~BulletRigidbody()
	{
		m_PhysicsContext->GetDynamicsWorld()->removeRigidBody(m_Rigidbody);

		delete m_Rigidbody;
		delete m_Shape;
	}

	void BulletRigidbody::AddCollider(const glm::vec3& position, const glm::quat& rotation, Collider* collider)
	{
		BulletCollider* bulletCollider = static_cast<BulletCollider*>(collider);

		btCollisionShape* shape = bulletCollider->GetShape();

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin({ position.x, position.y, position.z });

		m_Shape->addChildShape(transform, shape);
	}

	void BulletRigidbody::RemoveCollider(Collider* collider)
	{
		BulletCollider* bulletCollider = static_cast<BulletCollider*>(collider);

		btCollisionShape* shape = bulletCollider->GetShape();

		m_Shape->removeChildShape(shape);

		m_Rigidbody->setCollisionShape(m_Shape);
	}

	void BulletRigidbody::SetPositionRotation(const glm::vec3& pos, const glm::quat& rot)
	{
		btTransform transform = m_Rigidbody->getCenterOfMassTransform();
		transform.setOrigin({ pos.x, pos.y, pos.z });
		transform.setRotation({ rot.x, rot.y, rot.z, rot.w });
		m_Rigidbody->setCenterOfMassTransform(transform);
	}

	void BulletRigidbody::SetMass(float mass)
	{
		// TODO

		//m_Rigidbody->setMassProps(mass);
	}

	float BulletRigidbody::GetMass() const
	{
		return m_Rigidbody->getMass();
	}

	void BulletRigidbody::SetVelocity(const glm::vec3& velocity)
	{
		m_Rigidbody->setLinearVelocity({ velocity.x, velocity.y, velocity.z });
	}

	glm::vec3 BulletRigidbody::GetVelocity() const
	{
		auto& vel = m_Rigidbody->getLinearVelocity();

		return { vel.x(), vel.y(), vel.z() };
	}
}