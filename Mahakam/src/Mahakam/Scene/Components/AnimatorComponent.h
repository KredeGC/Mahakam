#pragma once

#include "Mahakam/Renderer/Animator.h"

namespace Mahakam
{
	struct AnimatorComponent
	{
	private:
		Animator m_Animator;

	public:
		AnimatorComponent() = default;

		AnimatorComponent(const AnimatorComponent&) = default;

		AnimatorComponent(Asset<Animation> animation)
		{
			m_Animator.SetAnimation(std::move(animation));
		}

		inline operator Animator& () { return m_Animator; }
		inline operator const Animator& () const { return m_Animator; }

		inline Animator& GetAnimator() { return m_Animator; }
	};
}