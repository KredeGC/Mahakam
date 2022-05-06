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

		AudioSourceComponent(Ref<Sound> sound)
		{
			m_Source->SetSound(sound);
		}

		void SetSound(Ref<Sound> sound) { m_Source->SetSound(sound); }
		Ref<Sound> GetSound() { return m_Source->GetSound(); }

		void Play() { m_Source->Play(); }

		Ref<AudioSource> GetAudioSource() { return m_Source; }
	};
}