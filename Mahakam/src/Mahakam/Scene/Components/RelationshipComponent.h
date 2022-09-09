#pragma once

#include <entt/entt.hpp>

namespace Mahakam
{
	struct RelationshipComponent
	{
		entt::entity First{ entt::null };
		entt::entity Prev{ entt::null };
		entt::entity Next{ entt::null };
		entt::entity Parent{ entt::null };

	public:
		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;
	};
}