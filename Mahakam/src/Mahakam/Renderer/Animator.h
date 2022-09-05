#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

namespace Mahakam
{
	class Animation;
	class Animator;

	extern template class Asset<Animator>;

	class Animator
	{
	private:
		Asset<Animation> m_Animation;

	public:
		Animator() = default;

		Asset<Animation> GetAnimation() const { return m_Animation; }
	};
}