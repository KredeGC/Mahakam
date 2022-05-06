#pragma once

#include "Mahakam/Core/Core.h"
#include "AudioContext.h"
#include "AudioSource.h"
#include "Sound.h"

#include <string>

namespace Mahakam
{
	class AudioEngine
	{
	private:
		static AudioContext* m_Context;

	public:
		static void Init();
		static void Shutdown();

		inline static void UpdateSounds(const glm::vec3& listener) { UpdateSoundsImpl(listener); }

		inline static AudioContext* GetContext() { return GetContextImpl(); }

	private:
		MH_DECLARE_FUNC(UpdateSoundsImpl, void, const glm::vec3& listener);

		MH_DECLARE_FUNC(GetContextImpl, AudioContext*);
	};
}