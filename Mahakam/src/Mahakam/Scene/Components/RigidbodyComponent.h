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
		RigidbodyComponent()
			: m_Rigidbody(Rigidbody::Create())
		{ }

		RigidbodyComponent(const RigidbodyComponent&) = default;

		Ref<Rigidbody> GetRigidbody() const { return m_Rigidbody; }
	};
}