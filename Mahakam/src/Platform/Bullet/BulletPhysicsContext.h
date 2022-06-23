#pragma once

#include "Mahakam/Physics/PhysicsContext.h"

#include <btBulletDynamicsCommon.h>

namespace Mahakam
{
	class BulletPhysicsContext : public PhysicsContext
	{
	private:
		btDefaultCollisionConfiguration* m_CollisionConfiguration;
		btCollisionDispatcher* m_Dispatcher;
		btBroadphaseInterface* m_OverlappingPairCache;
		btSequentialImpulseConstraintSolver* m_Solver;
		btDiscreteDynamicsWorld* m_DynamicsWorld;

	public:
		BulletPhysicsContext();
		~BulletPhysicsContext();

		virtual void Update(Timestep ts) override;

		btDiscreteDynamicsWorld* GetDynamicsWorld() const { return m_DynamicsWorld; }
	};
}