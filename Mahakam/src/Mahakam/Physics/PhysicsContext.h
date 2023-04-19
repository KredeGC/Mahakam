#pragma once

#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
	struct PhysicsProps
	{
		float Timestep = 1.0f / 60.0f;
	};

	class PhysicsContext
	{
	public:
		virtual ~PhysicsContext() = default;

		virtual void Update(Timestep ts) = 0;

		inline static PhysicsContext* Create(const PhysicsProps& props = PhysicsProps()) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, PhysicsContext*, const PhysicsProps& props);
	};
}