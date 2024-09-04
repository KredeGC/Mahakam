#pragma once

#include "AudioEngine.h"
#include "Sound.h"

namespace Mahakam
{
	class AudioDataSource
	{
	public:
		virtual ~AudioDataSource() = default;

		virtual void* GetDataSource() = 0;

		static Scope<AudioDataSource> Create(Asset<Sound> sound, AudioContext* context = AudioEngine::GetContext());
	};
}