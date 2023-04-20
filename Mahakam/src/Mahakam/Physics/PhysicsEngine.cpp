#include "Mahakam/mhpch.h"
#include "PhysicsEngine.h"
#include "PhysicsContext.h"

#include "Mahakam/Core/Allocator.h"

#include "Mahakam/Scene/Components/BoxColliderComponent.h"
#include "Mahakam/Scene/Components/FlagComponents.h"
#include "Mahakam/Scene/Components/RigidbodyComponent.h"

namespace Mahakam
{
	Scope<PhysicsContext> PhysicsEngine::s_Context = nullptr;

	void PhysicsEngine::Init()
	{
		s_Context = PhysicsContext::Create();
	}

	void PhysicsEngine::Shutdown()
	{
		s_Context = nullptr;
	}

	//void PhysicsEngine::Update(Timestep ts)
	MH_DEFINE_FUNC(PhysicsEngine::Update, void, Timestep ts)
	{
		// TODO: Go through each collider component and check its rigidbody
		// If it has no rigidbody, add it as a collider to the entity rigidbody
		// Mark it as ColliderDirtyComponent
		// Go through all ColliderDirtyComponent and recreate their compound colliders

		s_Context->Update(ts);

		//float dt = ts.GetSeconds();

		//// Clamp delta time
		//if (dt > 0.25)
		//	dt = 0.25;

		//s_Accumulator += dt;

		//// Fixed timestep
		//while (s_Accumulator >= s_Timestep)
		//{
		//	s_Accumulator -= s_Timestep;
		//	s_Context->Update(ts);
		//}
	};

	//PhysicsContext* PhysicsEngine::GetContext()
	MH_DEFINE_FUNC(PhysicsEngine::GetContext, PhysicsContext*)
	{
		return s_Context.get();
	};
}