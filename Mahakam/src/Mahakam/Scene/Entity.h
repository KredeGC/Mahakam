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
		operator entt::entity() const { return handle; }
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
		T& AddComponent(Args&&... args)
		{
			MH_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

			T& component = scene->registry.emplace<T>(handle, std::forward<Args>(args)...);

			scene->OnComponentAdded<T>(*this, component);

			return component;
		}

		template<typename T>
		T& GetComponent() const
		{
			MH_CORE_ASSERT(HasComponent<T>(), "Entity has no such component!");

			return scene->registry.get<T>(handle);
		}

		template<typename T>
		bool RemoveComponent()
		{
			MH_CORE_ASSERT(HasComponent<T>(), "Entity has no such component!");

			return scene->registry.remove<T>(handle);
		}

		template<typename T>
		bool HasComponent() const
		{
			return scene->registry.any_of<T>(handle);
		}
	};
}