#pragma once

#include "Animation.h"

namespace Mahakam
{
	class Animator
	{
	public:
		static constexpr int MAX_BONES = 100;

	private:
		UnorderedMap<std::string, Bone> m_Bones;
		std::vector<BoneTransform> m_BoneHierarchy;

		glm::mat4 m_FinalBoneMatrices[MAX_BONES];
		Asset<Animation> m_CurrentAnimation;
		float m_CurrentTime;

	public:
		Animator() = default;

		Animator(Asset<Animation> animation);

		void LoadAnimation(Asset<Animation> animation);

		void UpdateAnimation(float dt);

		void PlayAnimation();

		Bone* FindBone(const std::string& name);

		inline float GetTime() const { return m_CurrentTime; }

		inline Asset<Animation> GetAnimation() { return m_CurrentAnimation; }

		inline const glm::mat4* GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }

	private:
		void CalculateBoneTransforms();

		void ConstructBones(const aiAnimation* animation, const UnorderedMap<std::string, BoneInfo>& boneInfoMap);
	};
}