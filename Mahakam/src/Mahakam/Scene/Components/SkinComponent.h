#pragma once

#include "Mahakam/Scene/Entity.h"

#include <vector>

namespace Mahakam
{
	struct SkinComponent
	{
	private:
		/*UnorderedMap<uint32_t, Entity> m_BoneEntities;
		UnorderedMap<uint32_t, BoneInfo> m_Bones;*/

		std::vector<Entity> m_BoneEntities;
		std::vector<BoneInfo> m_Bones;

	public:
		SkinComponent() {}

		SkinComponent(const SkinComponent&) = default;

		inline void AddBoneEntity(const SkinnedMesh& skinnedMesh, const std::string& name, Entity entity)
		{
			m_BoneEntities.push_back(entity);
			m_Bones.push_back(skinnedMesh.boneInfo.at(name));
		}

		inline void AddBoneEntity(const BoneInfo& bone, Entity entity)
		{
			m_BoneEntities.push_back(entity);
			m_Bones.push_back(bone);
		}

		inline std::vector<Entity>& GetBoneEntities() { return m_BoneEntities; }
		inline const std::vector<BoneInfo>& GetBoneInfo() const { return m_Bones; }
	};
}