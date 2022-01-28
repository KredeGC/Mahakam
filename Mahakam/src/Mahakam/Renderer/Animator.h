#pragma once

#include "Animation.h"

namespace Mahakam
{
	class Animator
	{
	public:
		// TODO: Make into a component
		// Or alternatively add a component that uses this class


		// TODO: Implement this idea:
		/*Dear Ankit,

		I agree with the approach suggested by Ndaba. Storing the node hierarchy, either as an Assimp object or as your own object structure, is wasteful and unnecessary.
		This is because you have to recursively traverse the tree to find nodes, which not only is CPU cache-unfriendly,
		as you're jumping all over the place in your tree to compare a bunch of strings to each other, but it also makes the code unnecessarily complex.

		A much more efficient and simple way is to simply store the nodes in an array. For every node, you can store the index of its parent node.
		You only have to find parent nodes once, when you import your scene. Importantly, parent nodes should be stored first in the array, and their children after.
		This is easy if you traverse the tree from the root towards the leaves.

		Because parent nodes were stored first, when you are looking at a certain node to calculate its transforms, you can be sure the transforms for its parents have already been calculated,
		and you can simply access them as bones[child_bone->parent_index].
		Therefore, you can simply iterate over the array from start to finish and construct all the transforms!
		You don't have to traverse any kind of tree, or have any recursive function calls.

		With this approach, everything is structured in a much simpler way (an array),
		which makes the code much easier to work with, and also makes things much faster for the CPU to process.

		Hope that was helpful! :)*/


		Animator::Animator()
			: m_CurrentTime(0.0f)
		{
			m_FinalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
				m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		Animator::Animator(Animation* currentAnimation)
			: m_CurrentTime(0.0f), m_CurrentAnimation(currentAnimation)
		{
			m_FinalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
				m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		void Animator::UpdateAnimation(float dt)
		{
			m_DeltaTime = dt;
			if (m_CurrentAnimation)
			{
				m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
				m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
				CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
			}
		}

		void Animator::PlayAnimation(Ref<Animation> animation)
		{
			m_CurrentAnimation = animation;
			m_CurrentTime = 0.0f;
		}

		void Animator::CalculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform)
		{
			std::string nodeName = node->name;
			glm::mat4 nodeTransform = node->transformation;

			Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

			if (Bone)
			{
				Bone->Update(m_CurrentTime);
				nodeTransform = Bone->GetLocalTransform();
			}

			glm::mat4 globalTransformation = parentTransform * nodeTransform;

			auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
			if (boneInfoMap.find(nodeName) != boneInfoMap.end())
			{
				int index = boneInfoMap.at(nodeName).id;
				const glm::mat4& offset = boneInfoMap.at(nodeName).offset;
				m_FinalBoneMatrices[index] = globalTransformation * offset;
			}

			for (int i = 0; i < node->childrenCount; i++)
				CalculateBoneTransform(&node->children[i], globalTransformation);
		}

		const std::vector<glm::mat4>& GetFinalBoneMatrices()
		{
			return m_FinalBoneMatrices;
		}

	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;
		Ref<Animation> m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime = 0.0f;
	};
}