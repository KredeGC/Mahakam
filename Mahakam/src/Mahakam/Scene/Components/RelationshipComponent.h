#pragma once

#include "Mahakam/Scene/Entity.h"

namespace Mahakam
{
	struct RelationshipComponent
	{
		Entity First;
		Entity Prev;
		Entity Next;
		Entity Parent;

	public:
		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;
	};
}