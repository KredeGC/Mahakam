#pragma once

#include "Mahakam/Renderer/Animator.h"

namespace Mahakam
{
	struct AnimatorComponent
	{
	private:
		Animator animator;

	public:
		AnimatorComponent() = default;

		AnimatorComponent(const AnimatorComponent&) = default;

		AnimatorComponent(Asset<Animation> animation)
		{
			animator.SetAnimation(animation);
		}

		operator Animator& () { return animator; }
		operator const Animator& () const { return animator; }

		Animator& GetAnimator() { return animator; }
	};
}