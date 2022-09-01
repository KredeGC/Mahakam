#pragma once

#include "Mahakam/Core/Core.h"
#include "Scene.h"

#include <entt/entt.hpp>

namespace Mahakam
{
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
		operator Scene*() const { return scene; }

		bool operator==(const Entity& other) const
		{
			return handle == other.handle && scene == other.scene;
		}
		bool operator!=(const Entity& other) const
		{
			return !operator==(other);
		}

		void SetParent(Entity parent);
		Entity GetParent() const;
		void Delete();

		inline bool IsValid() const
		{
			if (scene)
				return scene->registry.valid(handle);
			return false;
		}

		template<typename T, typename... Args>
		void AddEmptyComponent(Args&&... args)
		{
			MH_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

			scene->registry.template emplace<T>(handle, std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			MH_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

			T& component = scene->registry.template emplace<T>(handle, std::forward<Args>(args)...);

			scene->OnComponentAdded<T>(*this, component);

			return component;
		}

		template<typename T>
		T* TryGetComponent() const
		{
			return scene->registry.template try_get<T>(handle);
		}

		template<typename T>
		T& GetComponent() const
		{
			MH_CORE_ASSERT(HasComponent<T>(), "Entity has no such component!");

			return scene->registry.template get<T>(handle);
		}

		template<typename T>
		bool RemoveComponent()
		{
			MH_CORE_ASSERT(HasComponent<T>(), "Entity has no such component!");

			return scene->registry.template remove<T>(handle);
		}

		template<typename T>
		bool HasComponent() const
		{
			return scene->registry.template any_of<T>(handle);
		}
	};
}

namespace std {
	template <>
	struct hash<Mahakam::Entity>
	{
		size_t operator()(const Mahakam::Entity& k) const
		{
			return hash<Mahakam::Scene*>()(k)
				^ (hash<uint32_t>()(k) << 1);
		}
	};

}