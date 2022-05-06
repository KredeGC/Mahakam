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
		{
			AudioContext* context = AudioEngine::GetContext();
			m_Source = AudioSource::Create(context);
		}

		AudioSourceComponent(const AudioSourceComponent&) = default;

		AudioSourceComponent(Ref<Sound> sound)
		{
			m_Source->SetSound(sound);
		}

		Ref<AudioSource> GetAudioSource() { return m_Source; }
	};
}