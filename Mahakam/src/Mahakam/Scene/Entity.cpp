#include "Mahakam/mhpch.h"
#include "Entity.h"

#include "Scene.h"

#include "Components/FlagComponents.h"
#include "Components/RelationshipComponent.h"

namespace Mahakam
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_Handle(handle), m_Scene(scene) {}

	Entity::operator bool() const
	{
		if (m_Handle == entt::null || !m_Scene)
			return false;
		return m_Scene->m_Registry.valid(m_Handle);
	}

	void Entity::SetParent(Entity parent)
	{
		auto& relation = m_Scene->m_Registry.get<RelationshipComponent>(m_Handle);

		// Clear our current parent
		ClearParent(*this, relation);

		// Clear the relationship
		relation.Prev = entt::null;
		relation.Next = entt::null;
		relation.Parent = parent.m_Handle;

		// If the want-to-be parent isn't valid
		if (!parent) return;

		auto& parentRelation = m_Scene->m_Registry.get<RelationshipComponent>(parent.m_Handle);

		// Get the last child of the parent
		entt::entity current = parentRelation.First;
		while (current != entt::null)
		{
			entt::entity next = m_Scene->m_Registry.get<RelationshipComponent>(current).Next;
			if (next != entt::null)
				current = next;
			else
				break;
		}

		if (current != entt::null)
		{
			// Append this entity to the last child
			m_Scene->m_Registry.get<RelationshipComponent>(current).Next = m_Handle;
			relation.Prev = current;
		}
		else
		{
			// If no children, append as the first
			parentRelation.First = m_Handle;
		}
	}

	Entity Entity::GetParent() const
	{
		return { m_Scene->m_Registry.get<RelationshipComponent>(m_Handle).Parent, m_Scene };
	}

	void Entity::SetFirstChild(Entity first)
	{
		auto& relation = m_Scene->m_Registry.get<RelationshipComponent>(m_Handle);
		auto& firstRelation = m_Scene->m_Registry.get<RelationshipComponent>(first.m_Handle);

		// Clear first's current parent
		ClearParent(first, firstRelation);

		// Clear their relationship
		firstRelation.Prev = entt::null;
		firstRelation.Next = entt::null;
		firstRelation.Parent = m_Handle;

		// Get our old first
		entt::entity oldFirst = relation.First;

		relation.First = first.m_Handle;

		if (oldFirst != entt::null)
		{
			firstRelation.Next = oldFirst;
			m_Scene->m_Registry.get<RelationshipComponent>(oldFirst).Prev = first.m_Handle;
		}
	}

	Entity Entity::GetFirstChild() const
	{
		return { m_Scene->m_Registry.get<RelationshipComponent>(m_Handle).First, m_Scene };
	}

	void Entity::SetNext(Entity next)
	{
		auto& relation = m_Scene->m_Registry.get<RelationshipComponent>(m_Handle);
		auto& nextRelation = m_Scene->m_Registry.get<RelationshipComponent>(next.m_Handle);

		// Clear next's current parent
		ClearParent(next, nextRelation);

		// Clear their relationship
		nextRelation.Prev = entt::null;
		nextRelation.Next = entt::null;
		nextRelation.Parent = relation.Parent;

		// Setting the next child of an empty parent is just unparenting
		if (relation.Parent == entt::null) return;

		if (relation.Next != entt::null)
		{
			nextRelation.Next = relation.Next;
			m_Scene->m_Registry.get<RelationshipComponent>(relation.Next).Prev = next.m_Handle;
		}

		relation.Next = next.m_Handle;
		nextRelation.Prev = m_Handle;
	}

	Entity Entity::GetNext() const
	{
		return { m_Scene->m_Registry.get<RelationshipComponent>(m_Handle).Next, m_Scene };
	}

	Entity Entity::GetPrev() const
	{
		return { m_Scene->m_Registry.get<RelationshipComponent>(m_Handle).Prev, m_Scene };
	}

	void Entity::Delete()
	{
		auto& relation = m_Scene->m_Registry.get<RelationshipComponent>(m_Handle);

		// Clear our current parent
		ClearParent(*this, relation);

		// Mark children for deletion
		// No need to reset their parents, they will soon be gone :)
		MarkForDeletion(*this, relation);
	}

	void Entity::ClearParent(Entity entity, RelationshipComponent& relation)
	{
		if (relation.Parent != entt::null)
		{
			auto& parentRelation = entity.m_Scene->m_Registry.get<RelationshipComponent>(relation.Parent);

			entt::entity prev = relation.Prev;
			entt::entity next = relation.Next;

			// Update the surrounding children
			if (prev != entt::null)
				entity.m_Scene->m_Registry.get<RelationshipComponent>(prev).Next = next;
			if (next != entt::null)
				entity.m_Scene->m_Registry.get<RelationshipComponent>(next).Prev = prev;
			if (parentRelation.First == entity.m_Handle)
				parentRelation.First = next;
		}
	}

	void Entity::MarkForDeletion(Entity entity, RelationshipComponent& relation)
	{
		// Recursively mark children for deletion
		entt::entity current = relation.First;
		while (current != entt::null)
		{
			auto& currentRelation = entity.m_Scene->m_Registry.get<RelationshipComponent>(current);

			Entity currentEntity{ current, entity.m_Scene };
			MarkForDeletion(currentEntity, currentRelation);

			current = currentRelation.Next;
		}

		if (!entity.m_Scene->m_Registry.any_of<DeleteComponent>(entity.m_Handle))
			entity.m_Scene->m_Registry.emplace<DeleteComponent>(entity.m_Handle);
	}
}