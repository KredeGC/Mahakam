#pragma once

#include "Mahakam/Scene/Entity.h"

namespace Mahakam
{
	struct RelationshipComponent
	{
		size_t Children = 0; // TODO: Incorporate somehow
		Entity First;
		Entity Prev;
		Entity Next;
		Entity Parent;

	public:
		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;
	};
}