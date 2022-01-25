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

		AnimatorComponent(Ref<Animation> animation)
		{
			animator.PlayAnimation(animation);
		}

		Animator& getAnimator() { return animator; }
	};
}