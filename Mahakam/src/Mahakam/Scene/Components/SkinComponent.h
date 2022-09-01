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
		std::vector<std::string> m_BoneNames;

	public:
		SkinComponent() {}

		SkinComponent(const SkinComponent&) = default;

		inline void SetSkin(const SkinnedMesh& skinnedMesh)
		{
			m_BoneEntities.reserve(skinnedMesh.boneCount);
			m_Bones.reserve(skinnedMesh.boneCount);
			m_BoneNames.reserve(skinnedMesh.boneCount);

			for (auto& node : skinnedMesh.BoneHierarchy)
			{
				auto& bone = skinnedMesh.boneInfo.at(node.name);

				m_Bones.push_back(bone);
				m_BoneNames.push_back(node.name);
			}
		}

		inline void AddBoneEntity(Entity entity)
		{
			m_BoneEntities.push_back(entity);
		}

		inline std::vector<Entity>& GetBoneEntities() { return m_BoneEntities; }
		inline std::vector<BoneInfo>& GetBoneInfo() { return m_Bones; }
		inline std::vector<std::string>& GetBoneNames() { return m_BoneNames; }
	};
}