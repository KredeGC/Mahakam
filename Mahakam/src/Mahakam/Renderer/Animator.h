#pragma once

#include "Animation.h"

namespace Mahakam
{
	class Animator
	{
	public:
		static constexpr int MAX_BONES = 100;

	private:
		std::vector<BoneTransform> m_BoneHierarchy;
		glm::mat4 m_FinalBoneMatrices[MAX_BONES];
		Ref<Animation> m_CurrentAnimation;
		float m_CurrentTime;

	public:
		Animator() = default;

		Animator(Ref<Animation> currentAnimation)
			: m_CurrentTime(0.0f), m_CurrentAnimation(currentAnimation), m_BoneHierarchy(m_CurrentAnimation->GetBoneHierarchy()) { }

		void UpdateAnimation(float dt);

		void PlayAnimation(Ref<Animation> animation);

		inline const glm::mat4* GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }

	private:
		void CalculateBoneTransforms();
	};
}