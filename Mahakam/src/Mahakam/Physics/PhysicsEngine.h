#pragma once

#include "Mahakam/Core/SharedLibrary.h"

namespace Mahakam
{
	class PhysicsContext;

	class PhysicsEngine
	{
	private:
		static PhysicsContext* s_Context;

	public:
		static void Init();
		static void Shutdown();

		MH_DECLARE_FUNC(Update, void, Timestep ts);
	};
}