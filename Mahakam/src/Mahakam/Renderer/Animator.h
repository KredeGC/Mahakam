#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

namespace Mahakam
{
	class Animation;
	class Animator;

	extern template class Asset<Animation>;

	class Animator
	{
	private:
		Asset<Animation> m_Animation;

		float m_Time = 0.0f;

		darray<size_t> m_Indices;

		UnorderedMap<int, glm::vec3> m_Translations;
		UnorderedMap<int, glm::quat> m_Rotations;
		UnorderedMap<int, glm::vec3> m_Scales;

	public:
		Animator() = default;

		void SetAnimation(Asset<Animation> animation);

		Asset<Animation> GetAnimation() const { return m_Animation; }

		float GetTime() const { return m_Time; }

		const UnorderedMap<int, glm::vec3>& GetTranslations() const { return m_Translations; }
		const UnorderedMap<int, glm::quat>& GetRotations() const { return m_Rotations; }
		const UnorderedMap<int, glm::vec3>& GetScales() const { return m_Scales; }

		void Update(Timestep dt);
	};
}