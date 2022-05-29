#pragma once

#include "btBulletDynamicsCommon.h"

namespace Mahakam
{
	class PhysicsEngine
	{
	private:
		static btDefaultCollisionConfiguration* m_CollisionConfiguration;
		static btCollisionDispatcher* m_Dispatcher;
		static btBroadphaseInterface* m_OverlappingPairCache;
		static btSequentialImpulseConstraintSolver* m_Solver;
		static btDiscreteDynamicsWorld* m_DynamicsWorld;

	public:
		static void Init();
		static void Shutdown();

		static void Update();
	};
}