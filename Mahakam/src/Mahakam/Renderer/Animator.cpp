#include "mhpch.h"
#include "Animator.h"

#include "Assimp.h"

namespace Mahakam
{
	Animator::Animator(Asset<Animation> animation)
	{
		LoadAnimation(animation);
	}

	void Animator::LoadAnimation(Asset<Animation> animation)
	{
		MH_PROFILE_FUNCTION();

		m_BoneHierarchy = animation->GetBoneHierarchy();

		m_CurrentAnimation = animation;
		m_CurrentTime = 0.0f;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_CurrentAnimation->GetFilepath().string(), aiProcess_Triangulate);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			MH_CORE_WARN("Could not load animation \"{0}\": {1}", m_CurrentAnimation->GetFilepath().string(), importer.GetErrorString());
		}
		else
		{
			auto animation = scene->mAnimations[0];

			ConstructBones(animation, m_CurrentAnimation->GetBoneIDMap());
		}
	}

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

	void Animator::PlayAnimation()
	{
		m_CurrentTime = 0;
	}

	Bone* Animator::FindBone(const std::string& name)
	{
		auto iter = m_Bones.find(name);
		if (iter != m_Bones.end())
			return &iter->second;

		return nullptr;
	}

	void Animator::CalculateBoneTransforms()
	{
		MH_PROFILE_FUNCTION();

		for (auto& node : m_BoneHierarchy)
		{
			std::string nodeName = node.name;
			glm::mat4 nodeTransform = node.transformation;

			auto iter = m_Bones.find(nodeName);
			if (iter != m_Bones.end())
			{
				iter->second.Update(m_CurrentTime);
				nodeTransform = iter->second.GetLocalTransform();
			}

			glm::mat4 parentTransform = node.parentIndex >= 0 ? m_BoneHierarchy[node.parentIndex].transformation : glm::mat4(1.0f);
			glm::mat4 globalTransformation = parentTransform * nodeTransform;

			node.transformation = globalTransformation;

			auto& boneInfo = m_CurrentAnimation->GetBoneIDMap();
			if (boneInfo.find(nodeName) != boneInfo.end())
			{
				int index = boneInfo.at(nodeName).id;
				const glm::mat4& offset = boneInfo.at(nodeName).offset;
				m_FinalBoneMatrices[index] = globalTransformation * offset;
			}
		}
	}

	void Animator::ConstructBones(const aiAnimation* animation, const UnorderedMap<std::string, BoneInfo>& boneInfoMap)
	{
		MH_PROFILE_FUNCTION();

		int size = animation->mNumChannels;

		// Reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.C_Str();

			// If this bone was loaded in the model
			auto iter = boneInfoMap.find(boneName);
			if (iter != boneInfoMap.end())
				m_Bones[boneName] = Bone(boneName, iter->second.id, channel);
		}
	}
}