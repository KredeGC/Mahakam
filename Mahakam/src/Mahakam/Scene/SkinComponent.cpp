#include "Mahakam/mhpch.h"

#include "Components/SkinComponent.h"
#include "Components/TransformComponent.h"

namespace Mahakam
{
	void SkinComponent::CreateBoneEntities(Entity entity, const std::vector<MeshNode>& hierarchy)
	{
		m_BoneEntities.clear();

		UnorderedMap<uint32_t, Entity> boneEntities;
		boneEntities.reserve(hierarchy.size());

		for (auto& node : hierarchy)
		{
			Entity boneEntity = entity.GetScene()->CreateEntity(node.Name);

			if (node.ParentID > -1)
				boneEntity.SetParent(boneEntities[node.ParentID]);
			else
				boneEntity.SetParent(entity);

			// Extract matrix
			glm::vec3 pos, scale;
			glm::quat rot;

			Entity parent = boneEntity.GetParent();
			glm::mat4 transform{ 1.0f };
			if (node.ParentID > -1)
				transform *= glm::inverse(parent.GetComponent<TransformComponent>().GetModelMatrix());
			transform *= glm::inverse(node.Offset);

			Math::DecomposeTransform(transform, pos, rot, scale);

			// Set transform
			boneEntity.AddComponent<TransformComponent>(pos, rot, scale);

			if (node.ParentID > -1)
				boneEntity.GetComponent<TransformComponent>().UpdateModelMatrix(parent.GetComponent<TransformComponent>().GetModelMatrix());
			else
				boneEntity.GetComponent<TransformComponent>().UpdateModelMatrix(glm::mat4(1.0f));

			m_BoneEntities.push_back(std::move(boneEntity));

			boneEntities[node.ID] = boneEntity;
		}

		entity.GetScene()->Sort();
	}
}