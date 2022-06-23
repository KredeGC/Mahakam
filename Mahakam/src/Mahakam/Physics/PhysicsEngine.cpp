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
		s_Context->Update(ts);
	};

	//PhysicsContext* PhysicsEngine::GetContext()
	MH_DEFINE_FUNC(PhysicsEngine::GetContext, PhysicsContext*)
	{
		return s_Context;
	};
}