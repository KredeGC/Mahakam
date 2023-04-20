#pragma once

#include "BulletCollider.h"
#include "Mahakam/Physics/Colliders/BoxCollider.h"
#include "Mahakam/Physics/Rigidbody.h"

#include <btBulletDynamicsCommon.h>

namespace Mahakam
{
	class BulletBoxCollider : public BoxCollider, public BulletCollider
	{
	private:
		Rigidbody* m_Rigidbody;

		btBoxShape m_Shape;
		glm::vec3 m_Extents;

	public:
		BulletBoxCollider(const glm::vec3& extents);
		~BulletBoxCollider();

		virtual void SetRigidbody(Rigidbody* rigidbody) override { m_Rigidbody = rigidbody; }
		virtual Rigidbody* GetRigidbody() override { return m_Rigidbody; }

		virtual void SetSize(const glm::vec3& extents) override;
		virtual const glm::vec3& GetSize() const override { return m_Extents; }

		virtual btCollisionShape* GetShape() override { return &m_Shape; }
	};
}