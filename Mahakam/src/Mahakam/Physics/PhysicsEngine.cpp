#include "mhpch.h"
#include "PhysicsEngine.h"
#include "PhysicsContext.h"

namespace Mahakam
{
	PhysicsContext* PhysicsEngine::s_Context = nullptr;

	void PhysicsEngine::Init()
	{
		s_Context = PhysicsContext::Create();
	}

	void PhysicsEngine::Shutdown()
	{
		delete s_Context;
	}

	//void PhysicsEngine::Update()
	MH_DEFINE_FUNC(PhysicsEngine::Update, void, Timestep ts)
	{
		float dt = ts.GetSeconds();

		// Clamp delta time
		if (dt > 0.25)
			dt = 0.25;

		s_Accumulator += dt;

		// Fixed timestep
		while (s_Accumulator >= s_Timestep)
		{
			s_Accumulator -= s_Timestep;
			s_Context->Update(dt);
		}
	};

	//PhysicsContext* PhysicsEngine::GetContext()
	MH_DEFINE_FUNC(PhysicsEngine::GetContext, PhysicsContext*)
	{
		return s_Context;
	};
}