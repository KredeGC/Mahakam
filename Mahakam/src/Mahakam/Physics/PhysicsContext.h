#pragma once

#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Core/Types.h"

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

		inline static Scope<PhysicsContext> Create(const PhysicsProps& props = PhysicsProps()) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Scope<PhysicsContext>, const PhysicsProps& props);
	};
}