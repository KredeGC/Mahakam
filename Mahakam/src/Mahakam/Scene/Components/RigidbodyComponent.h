#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Physics/Rigidbody.h"

namespace Mahakam
{
	struct RigidbodyComponent
	{
	private:
		Ref<Rigidbody> m_Rigidbody;

	public:
		RigidbodyComponent() {}

		RigidbodyComponent(const RigidbodyComponent&) = default;
	};
}