#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Physics/Colliders/BoxCollider.h"

namespace Mahakam
{
	struct BoxColliderComponent
	{
	private:
		Ref<BoxCollider> m_Collider;

	public:
		BoxColliderComponent()
			: m_Collider(BoxCollider::Create())
		{ }

		BoxColliderComponent(const BoxColliderComponent&) = default;

		Ref<BoxCollider> GetCollider() const { return m_Collider; }
	};
}