#pragma once

#include "Mahakam/Audio/AudioEngine.h"
#include "Mahakam/Audio/AudioSource.h"
#include "Mahakam/Audio/Sound.h"

namespace Mahakam
{
	struct AudioSourceComponent
	{
	private:
		Ref<AudioSource> m_Source;

	public:
		AudioSourceComponent()
			: m_Source(AudioSource::Create())
		{ }

		AudioSourceComponent(const AudioSourceComponent&) = default;

		AudioSourceComponent(Asset<Sound> sound)
		{
			m_Source->SetSound(sound);
		}

		inline void Play() { m_Source->Play(); }
		inline void Stop() { m_Source->Stop(); }

		inline void SetSound(Asset<Sound> sound) { m_Source->SetSound(std::move(sound)); }
		inline Asset<Sound> GetSound() const { return m_Source->GetSound(); }

		inline void SetInterpolation(bool interpolate) { m_Source->SetInterpolation(interpolate); }
		inline bool GetInterpolation() const { return m_Source->GetInterpolation(); }

		inline void SetSpatialBlend(float blend) { m_Source->SetSpatialBlend(blend); }
		inline float GetSpatialBlend() const { return m_Source->GetSpatialBlend(); }

		inline operator Ref<AudioSource>() const { return m_Source; }

		inline Ref<AudioSource> GetAudioSource() { return m_Source; }
	};
}