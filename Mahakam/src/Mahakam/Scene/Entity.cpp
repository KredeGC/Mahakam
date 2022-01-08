#include "mhpch.h"
#include "Entity.h"

#include "Scene.h"

namespace Mahakam
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: handle(handle), scene(scene) {}
}