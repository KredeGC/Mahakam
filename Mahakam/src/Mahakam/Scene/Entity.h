#pragma once

#include "Mahakam/Core/Core.h"

#include <entt/ent.hpp>

namespace Mahakam
{
	class Scene;

	class Entity
	{
	private:
		entt::entity entity = entt::null;
		Scene* scene = nullptr;

	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& ent) = default;

		operator bool() const { return entity != entt::null; }

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			MH_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");

			return scene->registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent() const
		{
			MH_CORE_ASSERT(hasComponent<T>(), "Entity has no such component!");

			return scene->registry.get<T>(entity);
		}

		template<typename T>
		bool removeComponent()
		{
			MH_CORE_ASSERT(hasComponent<T>(), "Entity has no such component!");

			return scene->registry.remove<T>(entity);
		}

		template<typename T>
		bool hasComponent() const
		{
			return scene->registry.any_of<T>(entity);
		}
	};
}