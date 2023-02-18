#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"

#include <entt/entt.hpp>

namespace Mahakam
{
	class Scene;

	struct RelationshipComponent;

	class Entity
	{
	private:
		entt::entity m_Handle = entt::null;
		Scene* m_Scene = nullptr;

	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& ent) = default;

		explicit operator bool() const;

		bool operator==(const Entity& other) const
		{
			return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
		}
		bool operator!=(const Entity& other) const
		{
			return !operator==(other);
		}

		entt::entity GetEntt() const { return m_Handle; }
		uint32_t GetHandle() const { return static_cast<uint32_t>(m_Handle); }
		Scene* GetScene() const;

		void SetParent(Entity parent);
		Entity GetParent() const;
		void SetFirstChild(Entity first);
		Entity GetFirstChild() const;
		void SetNext(Entity next);
		Entity GetNext() const;
		Entity GetPrev() const;
		void Delete();

		template<typename T, typename... Args>
		void AddEmptyComponent(Args&&... args);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args);

		template<typename T>
		T* TryGetComponent() const;

		template<typename T>
		T& GetComponent() const;

		template<typename T>
		bool RemoveComponent();

		template<typename T>
		bool HasComponent() const;

	private:
		static void ClearParent(Entity entity, RelationshipComponent& relation);
		static void MarkForDeletion(Entity entity, RelationshipComponent& relation);
	};
}

#include "Scene.h"

namespace Mahakam
{
	template<typename T, typename... Args>
	void Entity::AddEmptyComponent(Args&&... args)
	{
		MH_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

		m_Scene->m_Registry.template emplace<T>(m_Handle, std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	T& Entity::AddComponent(Args&&... args)
	{
		MH_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

		T& component = m_Scene->m_Registry.template emplace<T>(m_Handle, std::forward<Args>(args)...);

		m_Scene->OnComponentAdded<T>(*this, component);

		return component;
	}

	template<typename T>
	T* Entity::TryGetComponent() const
	{
		return m_Scene->m_Registry.template try_get<T>(m_Handle);
	}

	template<typename T>
	T& Entity::GetComponent() const
	{
		MH_CORE_ASSERT(HasComponent<T>(), "Entity has no such component!");

		return m_Scene->m_Registry.template get<T>(m_Handle);
	}

	template<typename T>
	bool Entity::RemoveComponent()
	{
		MH_CORE_ASSERT(HasComponent<T>(), "Entity has no such component!");

		return m_Scene->m_Registry.template remove<T>(m_Handle);
	}

	template<typename T>
	bool Entity::HasComponent() const
	{
		return m_Scene->m_Registry.template any_of<T>(m_Handle);
	}
}

namespace std {
	template <>
	struct hash<::Mahakam::Entity>
	{
		size_t operator()(const ::Mahakam::Entity& k) const
		{
			return hash<::Mahakam::Scene*>()(k.GetScene())
				^ (hash<uint32_t>()(k.GetHandle()) << 1);
		}
	};
}