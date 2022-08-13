#include "Mahakam/mhpch.h"
#include "Entity.h"

#include "Scene.h"

#include "Components/RelationshipComponent.h"

namespace Mahakam
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: handle(handle), scene(scene) {}

	void Entity::SetParent(Entity parent)
	{
		auto& relation = GetComponent<RelationshipComponent>();
		auto& parentRelation = parent.GetComponent<RelationshipComponent>();

		// Clear our current parent
		RemoveParent();

		relation.Parent = parent;

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

	Entity Entity::GetParent(Entity parent) const
	{
		return GetComponent<RelationshipComponent>().Parent;
	}

	void Entity::RemoveParent()
	{
		auto& relation = GetComponent<RelationshipComponent>();

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
			if (parentRelation.First == *this)
				parentRelation.First = next;
		}

		// Clear the relationship
		relation.Prev = {};
		relation.Next = {};
		relation.Parent = {};
	}

	void Entity::DestroyChildren()
	{
		auto& relation = GetComponent<RelationshipComponent>();

		// Go through each child and destroy it
		Entity current = relation.First;
		while (current)
		{
			Entity next = current.GetComponent<RelationshipComponent>().Next;

			current.DestroyChildren();

			scene->registry.destroy(current.handle);

			current = next;
		}

		relation.First = {};
	}
}