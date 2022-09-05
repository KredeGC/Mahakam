#pragma once

#include "Mahakam/Core/Timestep.h"

namespace Mahakam
{
	class PhysicsContext
	{
	public:
		virtual ~PhysicsContext() = default;

		virtual void Update(Timestep ts) = 0;

		static PhysicsContext* Create();
	};
}