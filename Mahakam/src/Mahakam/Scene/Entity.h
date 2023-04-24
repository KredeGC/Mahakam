#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"

#include "SceneDef.h"

#include <entt/entity/entity.hpp>

#include <type_traits>

namespace Mahakam
{
	struct RelationshipComponent;
	struct TagComponent;
	struct DeleteComponent;
	struct DirtyRelationshipComponent;
	struct DirtyColliderComponent;

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
		decltype(auto) AddComponent(Args&&... args)
		{
			static_assert(!std::is_same_v<T, TagComponent>);
			static_assert(!std::is_same_v<T, RelationshipComponent>);
			static_assert(!std::is_same_v<T, DeleteComponent>);
			static_assert(!std::is_same_v<T, DirtyRelationshipComponent>);
			static_assert(!std::is_same_v<T, DirtyColliderComponent>);

			MH_ASSERT(!HasComponent<T>(), "Entity already has component!");

			if constexpr (!std::is_empty_v<T>)
			{
				T& component = m_Scene->m_Registry.template emplace<T>(m_Handle, std::forward<Args>(args)...);

				m_Scene->OnComponentAdded<T>(*this, component);

				return component;
			}
			else
			{
				m_Scene->m_Registry.template emplace<T>(m_Handle, std::forward<Args>(args)...);
			}
		}

		template<typename T>
		T* TryGetComponent() const
		{
			return m_Scene->m_Registry.template try_get<T>(m_Handle);
		}

		template<typename T>
		T& GetComponent() const
		{
			MH_ASSERT(HasComponent<T>(), "Entity has no such component!");

			return m_Scene->m_Registry.template get<T>(m_Handle);
		}

		template<typename T>
		bool RemoveComponent()
		{
			static_assert(!std::is_same_v<T, TagComponent>);
			static_assert(!std::is_same_v<T, RelationshipComponent>);
			static_assert(!std::is_same_v<T, DeleteComponent>);
			static_assert(!std::is_same_v<T, DirtyRelationshipComponent>);
			static_assert(!std::is_same_v<T, DirtyColliderComponent>);

			MH_ASSERT(HasComponent<T>(), "Entity has no such component!");

			return m_Scene->m_Registry.template remove<T>(m_Handle);
		}

		template<typename T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.template any_of<T>(m_Handle);
		}
	};
}

namespace std
{
	template<>
	struct hash<::Mahakam::Entity>
	{
		size_t operator()(const ::Mahakam::Entity& k) const
		{
			return hash<::Mahakam::Scene*>()(k.GetScene())
				^ (hash<uint32_t>()(k.GetHandle()) << 1);
		}
	};
}