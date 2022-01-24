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
			// TODO: Create animator with animation clip
		}

		Animator& getAnimator() { return animator; }
	};
}