#include "mhpch.h"
#include "AudioEngine.h"

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

	MH_DEFINE_FUNC(AudioEngine::UpdateSoundsImpl, void, const glm::vec3& listener)
	{
		m_Context->UpdateSounds(listener);
	};

	MH_DEFINE_FUNC(AudioEngine::GetContextImpl, AudioContext*)
	{
		return m_Context;
	};
}