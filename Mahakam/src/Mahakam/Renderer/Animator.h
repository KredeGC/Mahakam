#pragma once

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Timestep.h"
#include "Mahakam/Core/Types.h"

#include "Mahakam/Asset/Asset.h"

namespace Mahakam
{
	class Animation;

	class Animator
	{
	private:
		Asset<Animation> m_Animation;

		float m_Time = 0.0f;
        bool m_Playing = false;

		TrivialArray<size_t, Allocator::BaseAllocator<size_t>> m_Indices;

		UnorderedMap<int, glm::vec3, Allocator::BaseAllocator<std::pair<const int, glm::vec3>>> m_Translations;
		UnorderedMap<int, glm::quat, Allocator::BaseAllocator<std::pair<const int, glm::quat>>> m_Rotations;
		UnorderedMap<int, glm::vec3, Allocator::BaseAllocator<std::pair<const int, glm::vec3>>> m_Scales;

	public:
		Animator();

		void SetAnimation(Asset<Animation> animation, bool play = true);

		inline Asset<Animation> GetAnimation() const { return m_Animation; }
        
        inline void Play() { m_Playing = true; }
        inline void Pause() { m_Playing = false; }

		inline float GetTime() const { return m_Time; }

		inline const UnorderedMap<int, glm::vec3, Allocator::BaseAllocator<std::pair<const int, glm::vec3>>>& GetTranslations() const { return m_Translations; }
		inline const UnorderedMap<int, glm::quat, Allocator::BaseAllocator<std::pair<const int, glm::quat>>>& GetRotations() const { return m_Rotations; }
		inline const UnorderedMap<int, glm::vec3, Allocator::BaseAllocator<std::pair<const int, glm::vec3>>>& GetScales() const { return m_Scales; }

		void Update(Timestep dt);
	};
}