#include "Mahakam/mhpch.h"
#include "Animator.h"

#include "Mahakam/Core/Profiler.h"

#include "Animation.h"

namespace Mahakam
{
	Animator::Animator() :
		m_Indices(Allocator::GetAllocator<size_t>()),
		m_Translations(Allocator::GetAllocator<std::pair<const int, glm::vec3>>()),
		m_Rotations(Allocator::GetAllocator<std::pair<const int, glm::quat>>()),
		m_Scales(Allocator::GetAllocator<std::pair<const int, glm::vec3>>())
	{}

	void Animator::SetAnimation(Asset<Animation> animation, bool play)
	{
		m_Animation = std::move(animation);
		m_Time = 0.0f;
        m_Playing = play;

		m_Translations.clear();
		m_Rotations.clear();
		m_Scales.clear();

		if (!m_Animation) return;

		const auto& samplers = m_Animation->GetSamplers();

		m_Indices.resize(samplers.size());
		std::memset(m_Indices.data(), 0, m_Indices.size() * sizeof(size_t));
	}

	void Animator::Update(Timestep dt)
	{
		MH_PROFILE_FUNCTION();

		if (!m_Animation) return;
        if (!m_Playing) return;

		const auto& samplers = m_Animation->GetSamplers();

		m_Time += dt;

		if (m_Time > m_Animation->GetDuration())
		{
			m_Time -= m_Animation->GetDuration();
			std::memset(m_Indices.data(), 0, m_Indices.size() * sizeof(size_t));
		}

		for (size_t i = 0; i < samplers.size(); i++)
		{
			auto& sampler = samplers.at(i);
			const auto& timestamps = sampler.Timestamps;
			const auto& translations = sampler.Translations;
			const auto& rotations = sampler.Rotations;
			const auto& scales = sampler.Scales;

			size_t frameCount = timestamps.size();
			size_t& nextIndex = m_Indices[i];

			float next = timestamps.at(nextIndex);
			while (m_Time > next && (nextIndex + 1) < frameCount)
			{
				next = timestamps.at(++nextIndex);
			}

			size_t prevIndex = 0;
			if (nextIndex >= 1)
				prevIndex = nextIndex - 1;
			float prev = timestamps.at(prevIndex);

			float interpolation;
			if (nextIndex == 0 && m_Time < next)
				interpolation = 0.0f; // Force default position before keyframe
			else if (nextIndex == frameCount - 1 && m_Time > next)
				interpolation = 1.0f; // Force end position after keyframe
			else
				interpolation = (m_Time - prev) / (next - prev);

			for (auto& bone : translations)
				m_Translations[bone.first] = glm::mix(bone.second.at(prevIndex), bone.second.at(nextIndex), interpolation);
			for (auto& bone : rotations)
				m_Rotations[bone.first] = glm::slerp(bone.second.at(prevIndex), bone.second.at(nextIndex), interpolation);
			for (auto& bone : scales)
				m_Scales[bone.first] = glm::mix(bone.second.at(prevIndex), bone.second.at(nextIndex), interpolation);
		}
	}
}