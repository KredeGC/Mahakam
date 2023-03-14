#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Scene/Entity.h"

#include "Mahakam/Renderer/MeshProps.h"

#include <vector>

namespace Mahakam
{
	struct SkeletonComponent
	{
	private:
		TrivialVector<Entity, Allocator::BaseAllocator<Entity>> m_BoneEntities;

	public:
		SkeletonComponent() : m_BoneEntities(Allocator::GetAllocator<Entity>()) {}

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

		inline TrivialVector<Entity, Allocator::BaseAllocator<Entity>>& GetBoneEntities() { return m_BoneEntities; }
	};
}