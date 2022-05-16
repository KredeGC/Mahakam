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

		void Play() { m_Source->Play(); }
		void Stop() { m_Source->Stop(); }

		void SetSound(Asset<Sound> sound) { m_Source->SetSound(sound); }
		Asset<Sound> GetSound() const { return m_Source->GetSound(); }

		void SetInterpolation(bool interpolate) { m_Source->SetInterpolation(interpolate); }
		bool GetInterpolation() const { return m_Source->GetInterpolation(); }

		void SetSpatialBlend(float blend) { m_Source->SetSpatialBlend(blend); }
		float GetSpatialBlend() const { return m_Source->GetSpatialBlend(); }

		Ref<AudioSource> GetAudioSource() { return m_Source; }
	};
}