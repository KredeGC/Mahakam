#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Asset/Asset.h"

#include "AudioEngine.h"
#include "AudioContext.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	class Sound;

	class AudioSource
	{
	public:
		virtual void Play() = 0;
		virtual void Stop() = 0;
		
		virtual void SetSound(Asset<Sound> sound) = 0;
		virtual Asset<Sound> GetSound() = 0;
		
		virtual void SetSpatialBlend(float blend) = 0;
		virtual float GetSpatialBlend() = 0;

		virtual void SetPosition(const glm::vec3& source) = 0;

		inline static Ref<AudioSource> Create() { return CreateImpl(AudioEngine::GetContext()); }
		inline static Ref<AudioSource> Create(AudioContext* context) { return CreateImpl(context); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Ref<AudioSource>, AudioContext* context);
	};
}