#pragma once

#include "Mahakam/Audio/AudioEngine.h"
#include "Mahakam/Audio/AudioSource.h"
#include "Mahakam/Audio/Sound.h"

namespace Mahakam
{
	struct AudioSourceComponent
	{
	private:
		Scope<AudioSource> m_Source;

	public:
		AudioSourceComponent()
			: m_Source(AudioSource::Create())
		{ }

		AudioSourceComponent(const AudioSourceComponent& other) noexcept :
			m_Source(AudioSource::Create(other.GetContext()))
		{
			m_Source->SetSound(other.GetSound());
			m_Source->SetInterpolation(other.GetInterpolation());
			m_Source->SetSpatialBlend(other.GetSpatialBlend());
		}

		AudioSourceComponent(Asset<Sound> sound)
		{
			m_Source->SetSound(sound);
		}

		AudioSourceComponent& operator=(const AudioSourceComponent& other) noexcept
		{
			m_Source = AudioSource::Create(other.GetContext());

			m_Source->SetSound(other.GetSound());
			m_Source->SetInterpolation(other.GetInterpolation());
			m_Source->SetSpatialBlend(other.GetSpatialBlend());

			return *this;
		}

		inline void Play() { m_Source->Play(); }
		inline void Stop() { m_Source->Stop(); }

		inline void SetSound(Asset<Sound> sound) { m_Source->SetSound(std::move(sound)); }
		inline Asset<Sound> GetSound() const { return m_Source->GetSound(); }

		inline void SetInterpolation(bool interpolate) { m_Source->SetInterpolation(interpolate); }
		inline bool GetInterpolation() const { return m_Source->GetInterpolation(); }

		inline void SetSpatialBlend(float blend) { m_Source->SetSpatialBlend(blend); }
		inline float GetSpatialBlend() const { return m_Source->GetSpatialBlend(); }

		inline AudioContext* GetContext() const { return m_Source->GetContext(); }

		inline operator AudioSource&() const { return *m_Source; }

		inline AudioSource& GetAudioSource() { return *m_Source; }
	};
}