#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Scene/Entity.h"

#include "Mahakam/Renderer/MeshProps.h"

#include <vector>

namespace Mahakam
{
	struct SkinComponent
	{
	private:
		TrivialVector<Entity> m_BoneEntities;
		bool m_TargetOrigin = false;

	public:
		SkinComponent() {}

		inline void ClearBoneEntities(const std::vector<MeshNode>& hierarchy)
		{
			m_BoneEntities.clear();

			for (size_t i = 0; i < hierarchy.size(); i++)
				m_BoneEntities.push_back({});
		}

		inline void AddBoneEntity(Entity entity)
		{
			m_BoneEntities.push_back(std::move(entity));
		}

		void CreateBoneEntities(Entity entity, const std::vector<MeshNode>& hierarchy);

		inline void EnableTargetOrigin(bool enable) { m_TargetOrigin = enable; }
		inline bool HasTargetOrigin() const { return m_TargetOrigin; }

		inline TrivialVector<Entity>& GetBoneEntities() { return m_BoneEntities; }
	};
}