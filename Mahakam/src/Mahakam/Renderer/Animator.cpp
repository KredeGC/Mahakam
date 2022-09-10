#include "Mahakam/mhpch.h"
#include "Animator.h"

#include "Animation.h"

namespace Mahakam
{
	void Animator::SetAnimation(Asset<Animation> animation)
	{
		m_Animation = animation;
		m_Time = 0.0f;
		m_Index = 0;
	}

	void Animator::Update(Timestep dt)
	{
		if (!m_Animation) return;

		m_Time += dt;

		const auto& timestamps = m_Animation->GetTimestamps();
		const auto& translations = m_Animation->GetTranslations();
		const auto& rotations = m_Animation->GetRotations();
		const auto& scales = m_Animation->GetScales();

		float next = timestamps.at(m_Index);
		while (m_Time >= next)
		{
			m_Index++;

			if (m_Index >= timestamps.size())
			{
				m_Index -= timestamps.size();
				m_Time -= timestamps.at(timestamps.size() - 1);
			}

			next = timestamps.at(m_Index);
		}

		m_Translations.clear();
		m_Rotations.clear();
		m_Scales.clear();

		// TODO: Interpolate results

		for (auto& bone : translations)
			m_Translations[bone.first] = bone.second.at(m_Index);
		for (auto& bone : rotations)
			m_Rotations[bone.first] = bone.second.at(m_Index);
		for (auto& bone : scales)
			m_Scales[bone.first] = bone.second.at(m_Index);
	}
}