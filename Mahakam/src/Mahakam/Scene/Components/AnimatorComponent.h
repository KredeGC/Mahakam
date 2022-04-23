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

		AnimatorComponent(Ref<Animation> animation)
		{
			animator.PlayAnimation(animation);
		}

		operator Animator& () { return animator; }
		operator const Animator& () const { return animator; }

		Animator& GetAnimator() { return animator; }
	};
}