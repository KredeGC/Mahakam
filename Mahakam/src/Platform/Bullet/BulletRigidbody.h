#pragma once

#include "Mahakam/Physics/Colliders/Collider.h"
#include "Mahakam/Physics/Rigidbody.h"

class btRigidBody;
class btCompoundShape;

namespace Mahakam
{
	class PhysicsContext;
	class BulletPhysicsContext;

	class BulletRigidbody : public Rigidbody
	{
	private:
		BulletPhysicsContext* m_PhysicsContext;

		btRigidBody* m_Rigidbody;

		btCompoundShape* m_Shape;

	public:
		BulletRigidbody(PhysicsContext* context);
		~BulletRigidbody();

		virtual void AddCollider(const glm::vec3& position, const glm::quat& rotation, Collider* collider) override;
		virtual void RemoveCollider(Collider* collider) override;

		virtual void SetPositionRotation(const glm::vec3& pos, const glm::quat& rot) override;

		virtual void SetMass(float mass) override;
		virtual float GetMass() const override;

		virtual void SetVelocity(const glm::vec3 & velocity) override;
		virtual glm::vec3 GetVelocity() const override;
	};
}