#include "mhpch.h"
#include "Animator.h"

namespace Mahakam
{
	void Animator::UpdateAnimation(float dt)
	{
		MH_PROFILE_FUNCTION();

		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

			CalculateBoneTransforms();
		}
	}

	void Animator::PlayAnimation(Ref<Animation> animation)
	{
		MH_PROFILE_FUNCTION();

		m_CurrentAnimation = animation;
		m_CurrentTime = 0.0f;
		m_BoneHierarchy = m_CurrentAnimation->GetBoneHierarchy();
	}

	void Animator::CalculateBoneTransforms()
	{
		MH_PROFILE_FUNCTION();

		for (auto& node : m_BoneHierarchy)
		{
			std::string nodeName = node.name;
			glm::mat4 nodeTransform = node.transformation;

			Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

			if (Bone)
			{
				Bone->Update(m_CurrentTime);
				nodeTransform = Bone->GetLocalTransform();
			}

			glm::mat4 parentTransform = node.parentIndex >= 0 ? m_BoneHierarchy[node.parentIndex].transformation : glm::mat4(1.0f);
			glm::mat4 globalTransformation = parentTransform * nodeTransform;

			node.transformation = globalTransformation;

			auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
			if (boneInfoMap.find(nodeName) != boneInfoMap.end())
			{
				int index = boneInfoMap.at(nodeName).id;
				const glm::mat4& offset = boneInfoMap.at(nodeName).offset;
				m_FinalBoneMatrices[index] = globalTransformation * offset;
			}
		}
	}
}