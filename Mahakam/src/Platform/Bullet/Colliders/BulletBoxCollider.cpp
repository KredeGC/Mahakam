#include "Mahakam/mhpch.h"
#include "BulletBoxCollider.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	Ref<BoxCollider> BoxCollider::Create(const glm::vec3& extents)
	{
		return CreateRef<BulletBoxCollider>(extents);
	}

	BulletBoxCollider::BulletBoxCollider(const glm::vec3& extents)
		: m_Shape({ extents.x, extents.y, extents.z }), m_Extents(extents)
	{ }

	BulletBoxCollider::~BulletBoxCollider()
	{

	}

	void BulletBoxCollider::SetSize(const glm::vec3& extents)
	{
		m_Shape = btBoxShape({ extents.x, extents.y, extents.z });
		m_Extents = extents;
	}
}