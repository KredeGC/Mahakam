#pragma once

#include "Mahakam/Core/Core.h"

namespace Mahakam
{
	class Animation;

	class Animator
	{
	private:
		Asset<Animation> m_Animation;

	public:
		Animator() = default;

		Asset<Animation> GetAnimation() const { return m_Animation; }
	};
}