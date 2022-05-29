#include "ebpch.h"
#include "EditorApplication.h"
#include <Mahakam/Core/EntryPoint.h>

#include <imgui.h>

// TEMP
#include "btBulletDynamicsCommon.h"

namespace Mahakam::Editor
{
	EditorApplication::EditorApplication() : Application({ "Erebor", "res/internal/icons/icon-editor.png" })
	{
		// Create editor layer
		m_EditorLayer = new EditorLayer();

		PushOverlay(m_EditorLayer);

		//GetWindow().SetVSync(true);



		// TEMP
		///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -10, 0));
	}

	EditorApplication::~EditorApplication()
	{

	}
}

extern Mahakam::Application* Mahakam::CreateApplication()
{
	return new Mahakam::Editor::EditorApplication();
}