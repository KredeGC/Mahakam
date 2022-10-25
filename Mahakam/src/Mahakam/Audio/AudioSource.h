#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include "AudioEngine.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>

namespace Mahakam
{
	class AudioContext;
	class Sound;

	class AudioSource
	{
	public:
		virtual void Play() = 0;
		virtual void Stop() = 0;
		
		virtual void SetSound(Asset<Sound> sound) = 0;
		virtual Asset<Sound> GetSound() const = 0;

		virtual void SetInterpolation(bool interpolate) = 0;
		virtual bool GetInterpolation() const = 0;
		
		virtual void SetSpatialBlend(float blend) = 0;
		virtual float GetSpatialBlend() const = 0;

		virtual void SetPosition(const glm::vec3& source) = 0;

		inline static Ref<AudioSource> Create() { return CreateImpl(AudioEngine::GetContext()); }
		inline static Ref<AudioSource> Create(AudioContext* context) { return CreateImpl(context); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Ref<AudioSource>, AudioContext* context);
	};
}