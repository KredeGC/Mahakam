#pragma once

#include "Mahakam/Scene/Entity.h"

#include <vector>

namespace Mahakam
{
	struct SkinComponent
	{
	private:
		std::vector<Entity> m_BoneEntities;
		bool m_TargetOrigin = false;

	public:
		SkinComponent() {}

		inline void AddBoneEntity(Entity entity)
		{
			m_BoneEntities.push_back(std::move(entity));
		}

		inline void EnableTargetOrigin(bool enable) { m_TargetOrigin = enable; }
		inline bool HasTargetOrigin() const { return m_TargetOrigin; }

		inline std::vector<Entity>& GetBoneEntities() { return m_BoneEntities; }
	};
}