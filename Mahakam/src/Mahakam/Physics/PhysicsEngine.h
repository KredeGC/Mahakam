#pragma once

#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Timestep.h"

namespace Mahakam
{
	class PhysicsContext;

	class PhysicsEngine
	{
	private:
		static Scope<PhysicsContext> s_Context;
		inline static float s_Timestep = 1.0f / 60.0f;
		inline static float s_Accumulator = 0.0f;

	public:
		static void Init();
		static void Shutdown();

		MH_DECLARE_FUNC(Update, void, Timestep ts);

		MH_DECLARE_FUNC(GetContext, PhysicsContext*);
	};
}