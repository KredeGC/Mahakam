#pragma once

#include "Mahakam/Scene/Entity.h"

#include <vector>

namespace Mahakam
{
	struct SkinComponent
	{
	private:
		std::vector<Entity> m_BoneEntities;

	public:
		SkinComponent() {}

		inline void AddBoneEntity(Entity entity)
		{
			m_BoneEntities.push_back(entity);
		}

		inline std::vector<Entity>& GetBoneEntities() { return m_BoneEntities; }
	};
}