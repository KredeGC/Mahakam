#pragma once

#include "Mahakam/Physics/PhysicsContext.h"

#include <btBulletDynamicsCommon.h>

namespace Mahakam
{
	class BulletPhysicsContext : public PhysicsContext
	{
	private:
		PhysicsProps m_Props;

		btDefaultCollisionConfiguration* m_CollisionConfiguration;
		btCollisionDispatcher* m_Dispatcher;
		btBroadphaseInterface* m_OverlappingPairCache;
		btSequentialImpulseConstraintSolver* m_Solver;
		btDiscreteDynamicsWorld* m_DynamicsWorld;

	public:
		BulletPhysicsContext(const PhysicsProps& props);
		~BulletPhysicsContext();

		virtual void Update(Timestep ts) override;

		btDiscreteDynamicsWorld* GetDynamicsWorld() const { return m_DynamicsWorld; }
	};
}