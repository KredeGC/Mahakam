#pragma once

#include "Mahakam/Scene/Entity.h"

#include <vector>

namespace Mahakam
{
	struct SkinComponent
	{
	private:
		std::vector<Entity> m_BoneEntities;
		std::vector<BoneInfo> m_Bones;

	public:
		SkinComponent() {}

		SkinComponent(const SkinComponent&) = default;

		inline void SetSkin(const SkinnedMesh& skinnedMesh)
		{
			m_BoneEntities.reserve(skinnedMesh.boneCount);
			m_Bones.reserve(skinnedMesh.boneCount);

			for (auto& node : skinnedMesh.BoneHierarchy)
			{
				auto& bone = skinnedMesh.boneInfo.at(node.name);

				m_Bones.push_back(bone);
			}
		}

		inline void AddBoneEntity(Entity entity)
		{
			m_BoneEntities.push_back(entity);
		}

		inline std::vector<Entity>& GetBoneEntities() { return m_BoneEntities; }
		inline const std::vector<BoneInfo>& GetBoneInfo() const { return m_Bones; }
	};
}