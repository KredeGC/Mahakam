#include "mhpch.h"
#include "Entity.h"

#include "Scene.h"

namespace Mahakam
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: entity(handle), scene(scene) {}
}