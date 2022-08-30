#include "Mahakam/mhpch.h"
#include "Entity.h"

#include "Scene.h"

#include "Components/FlagComponents.h"
#include "Components/RelationshipComponent.h"

namespace Mahakam
{
	static void ClearParent(Entity entity, RelationshipComponent& relation)
	{
		if (relation.Parent)
		{
			auto& parentRelation = relation.Parent.GetComponent<RelationshipComponent>();

			Entity prev = relation.Prev;
			Entity next = relation.Next;

			// Update the surrounding children
			if (prev)
				prev.GetComponent<RelationshipComponent>().Next = next;
			if (next)
				next.GetComponent<RelationshipComponent>().Prev = prev;
			if (parentRelation.First == entity)
				parentRelation.First = next;
		}
	}

	static void MarkForDeletion(Entity entity, RelationshipComponent& relation)
	{
		// Recursively mark children for deletion
		Entity current = relation.First;
		while (current)
		{
			auto& currentRelation = current.GetComponent<RelationshipComponent>();

			MarkForDeletion(current, currentRelation);

			current = currentRelation.Next;
		}

		if (!entity.HasComponent<DeleteComponent>())
			entity.AddEmptyComponent<DeleteComponent>();
	}

	Entity::Entity(entt::entity handle, Scene* scene)
		: handle(handle), scene(scene) {}

	Entity::Entity(entt::entity handle, const Entity& entity)
		: handle(handle), scene(entity.scene) {}

	void Entity::SetParent(Entity parent)
	{
		auto& relation = GetComponent<RelationshipComponent>();

		// Clear our current parent
		ClearParent(*this, relation);

		// Clear the relationship
		relation.Prev = {};
		relation.Next = {};
		relation.Parent = parent;

		// If the want-to-be parent isn't valid
		if (!parent) return;

		auto& parentRelation = parent.GetComponent<RelationshipComponent>();

		// Get the last child of the parent
		Entity current = parentRelation.First;
		while (current)
		{
			Entity next = current.GetComponent<RelationshipComponent>().Next;
			if (next)
				current = next;
			else
				break;
		}

		if (current)
		{
			// Append this entity to the last child
			current.GetComponent<RelationshipComponent>().Next = *this;
			relation.Prev = current;
		}
		else
		{
			// If no children, append as the first
			parentRelation.First = *this;
		}
	}

	Entity Entity::GetParent() const
	{
		return GetComponent<RelationshipComponent>().Parent;
	}

	void Entity::Delete()
	{
		auto& relation = GetComponent<RelationshipComponent>();

		// Clear our current parent
		ClearParent(*this, relation);

		// Mark children for deletion
		// No need to reset their parents, they will soon be gone :)
		MarkForDeletion(*this, relation);
	}
}