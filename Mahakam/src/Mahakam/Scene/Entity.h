#pragma once

#include "Mahakam/Core/Core.h"

#include <entt/entt.hpp>

namespace Mahakam
{
	class Scene;

	class Entity
	{
	private:
		entt::entity handle = entt::null;
		Scene* scene = nullptr;

	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& ent) = default;

		operator bool() const { return handle != entt::null; }
		operator uint32_t() const { return (uint32_t)handle; }

		bool operator==(const Entity& other) const
		{
			return handle == other.handle && scene == other.scene;
		}
		bool operator!=(const Entity& other) const
		{
			return !operator==(other);
		}

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			MH_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");

			return scene->registry.emplace<T>(handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent() const
		{
			MH_CORE_ASSERT(hasComponent<T>(), "Entity has no such component!");

			return scene->registry.get<T>(handle);
		}

		template<typename T>
		bool removeComponent()
		{
			MH_CORE_ASSERT(hasComponent<T>(), "Entity has no such component!");

			return scene->registry.remove<T>(handle);
		}

		template<typename T>
		bool hasComponent() const
		{
			return scene->registry.any_of<T>(handle);
		}
	};
}