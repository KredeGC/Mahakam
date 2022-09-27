#include "Mahakam/mhpch.h"
#include "BulletPhysicsContext.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

namespace Mahakam
{
	PhysicsContext* PhysicsContext::Create()
	{
		return new BulletPhysicsContext;
	}

	BulletPhysicsContext::BulletPhysicsContext()
	{
		//collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		m_CollisionConfiguration = new btDefaultCollisionConfiguration();

		//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);

		//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		m_OverlappingPairCache = new btDbvtBroadphase();

		//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		m_Solver = new btSequentialImpulseConstraintSolver;

		m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_OverlappingPairCache, m_Solver, m_CollisionConfiguration);

		m_DynamicsWorld->setGravity(btVector3(0, -10, 0));


		//keep track of the shapes, we release memory at exit.
		//make sure to re-use collision shapes among rigid bodies whenever possible!
		//btAlignedObjectArray<btCollisionShape*> collisionShapes;
	}

	BulletPhysicsContext::~BulletPhysicsContext()
	{
		delete m_DynamicsWorld;
		delete m_Solver;
		delete m_OverlappingPairCache;
		delete m_Dispatcher;
		delete m_CollisionConfiguration;
	}

	void BulletPhysicsContext::Update(Timestep ts)
	{
		MH_PROFILE_FUNCTION();

		m_DynamicsWorld->stepSimulation(ts, 1, 1.0f / 60.0f);

		//print positions of all objects
		//for (int j = m_DynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		//{
		//	btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[j];
		//	btRigidBody* body = btRigidBody::upcast(obj);
		//	btTransform trans;
		//	if (body && body->getMotionState())
		//	{
		//		body->getMotionState()->getWorldTransform(trans);
		//	}
		//	else
		//	{
		//		trans = obj->getWorldTransform();
		//	}
		//	//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		//}
	}
}