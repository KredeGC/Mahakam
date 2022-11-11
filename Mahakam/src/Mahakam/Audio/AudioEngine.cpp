#include "Mahakam/mhpch.h"
#include "AudioEngine.h"

#include "AudioContext.h"

#include "Mahakam/Core/Allocator.h"

namespace Mahakam
{
	Scope<AudioContext> AudioEngine::s_Context;

	void AudioEngine::Init()
	{
		s_Context = AudioContext::Create();
	}

	void AudioEngine::Shutdown()
	{
		s_Context = nullptr;
	}

	MH_DEFINE_FUNC(AudioEngine::UpdateSoundsImpl, void, const glm::mat4& listenerTransform)
	{
		s_Context->UpdateSounds(listenerTransform);
	};

	MH_DEFINE_FUNC(AudioEngine::GetContextImpl, AudioContext*)
	{
		return s_Context.get();
	};
}