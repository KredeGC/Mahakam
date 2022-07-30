#pragma once

#include <string>

namespace Mahakam
{
	struct TagComponent
	{
		std::string Tag;

		uint64_t ID = 0;
		uint64_t ParentID = 0;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
		TagComponent(uint64_t ID, uint64_t parentID, const std::string& tag)
			: ID(ID), ParentID(parentID), Tag(tag) {}
	};
}