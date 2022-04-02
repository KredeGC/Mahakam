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

		Animator& GetAnimator() { return animator; }
	};
}