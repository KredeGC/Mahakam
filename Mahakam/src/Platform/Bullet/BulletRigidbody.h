#pragma once

#include "Mahakam/Physics/Rigidbody.h"

class btRigidBody;

namespace Mahakam
{
	class PhysicsContext;

	class BulletRigidbody : public Rigidbody
	{
	private:
		btRigidBody* m_Rigidbody;

		glm::vec3 m_Velocity; // TODO

	public:
		BulletRigidbody(const glm::vec3& position, float mass, PhysicsContext* context);
		~BulletRigidbody();

		virtual void SetMass(float mass) override;
		virtual float GetMass() const override;

		virtual void SetVelocity(const glm::vec3 & velocity) override;
		virtual const glm::vec3& GetVelocity() const override { return m_Velocity; }
	};
}