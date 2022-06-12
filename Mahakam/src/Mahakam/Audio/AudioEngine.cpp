#include "mhpch.h"
#include "AudioEngine.h"

#include "AudioContext.h"

namespace Mahakam
{
	AudioContext* AudioEngine::m_Context;

	void AudioEngine::Init()
	{
		m_Context = AudioContext::Create();
	}

	void AudioEngine::Shutdown()
	{
		delete m_Context;
	}

	MH_DEFINE_FUNC(AudioEngine::UpdateSoundsImpl, void, const glm::mat4& listenerTransform)
	{
		m_Context->UpdateSounds(listenerTransform);
	};

	MH_DEFINE_FUNC(AudioEngine::GetContextImpl, AudioContext*)
	{
		return m_Context;
	};
}