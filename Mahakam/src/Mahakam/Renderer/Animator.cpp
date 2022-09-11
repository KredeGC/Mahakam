#include "Mahakam/mhpch.h"
#include "Animator.h"

#include "Animation.h"

namespace Mahakam
{
	void Animator::SetAnimation(Asset<Animation> animation)
	{
		m_Animation = animation;
		m_Time = 0.0f;

		m_Indices.clear();
		for (auto& kv : animation->GetSamplers())
			m_Indices[kv.first] = 0;
	}

	void Animator::Update(Timestep dt)
	{
		if (!m_Animation) return;

		const auto& samplers = m_Animation->GetSamplers();

		m_Time += dt;

		if (m_Time > m_Animation->GetDuration())
		{
			m_Time -= m_Animation->GetDuration();
			//memset(m_Indices.data(), 0, m_Indices.size() * sizeof(uint32_t));
			for (auto& s : samplers)
				m_Indices[s.first] = 0;
		}

		m_Translations.clear();
		m_Rotations.clear();
		m_Scales.clear();

		for (auto& s : samplers)
		{
			auto& samplerID = s.first;
			auto& sampler = s.second;
			const auto& timestamps = sampler.Timestamps;
			const auto& translations = sampler.Translations;
			const auto& rotations = sampler.Rotations;
			const auto& scales = sampler.Scales;

			size_t& index = m_Indices[samplerID];

			float next = timestamps.at(index);
			while (m_Time >= next)
			{
				if (index + 1 < timestamps.size())
					next = timestamps.at(++index);
				else
					break;
			}

			// TODO: Interpolate results

			for (auto& bone : translations)
				m_Translations[bone.first] = bone.second.at(index);
			for (auto& bone : rotations)
				m_Rotations[bone.first] = bone.second.at(index);
			for (auto& bone : scales)
				m_Scales[bone.first] = bone.second.at(index);
		}
	}
}