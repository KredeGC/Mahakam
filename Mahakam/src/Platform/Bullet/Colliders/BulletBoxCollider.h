#pragma once

#include "Mahakam/Physics/Colliders/BoxCollider.h"

#include <btBulletDynamicsCommon.h>

namespace Mahakam
{
	class BulletBoxCollider : public BoxCollider
	{
	private:
		btBoxShape m_Shape;
		glm::vec3 m_Extents;

	public:
		BulletBoxCollider(const glm::vec3& extents);
		~BulletBoxCollider();

		virtual void SetSize(const glm::vec3& extents) override;
		virtual const glm::vec3& GetSize() const override { return m_Extents; }
	};
}