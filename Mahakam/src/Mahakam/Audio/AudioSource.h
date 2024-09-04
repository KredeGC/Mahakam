#pragma once

#include "SoundProps.h"

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include "AudioEngine.h"
#include "AudioDataSource.h"

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
		virtual ~AudioSource() = default;

		virtual void Play() = 0;
		virtual void Stop() = 0;
		
		virtual void SetDataSource(Scope<AudioDataSource> dataSource) = 0;
		virtual AudioDataSource* GetDataSource() const = 0;

		virtual const SoundProps& GetProps() const = 0;
		virtual void SetProps(const SoundProps& props) = 0;

		virtual void SetVolume(float volume) = 0;
		virtual float GetVolume() const = 0;

		virtual void SetLooping(bool loop) = 0;
		virtual bool GetLooping() const = 0;

		virtual void SetInterpolation(bool interpolate) = 0;
		virtual bool GetInterpolation() const = 0;
		
		virtual void SetSpatialBlend(float blend) = 0;
		virtual float GetSpatialBlend() const = 0;

		virtual float GetTime() const = 0;
		virtual float GetDuration() const = 0;

		virtual void SetPosition(const glm::vec3& source) = 0;

		virtual AudioContext* GetContext() const = 0;

		inline static Scope<AudioSource> Create() { return CreateImpl(AudioEngine::GetContext()); }
		inline static Scope<AudioSource> Create(AudioContext* context) { return CreateImpl(context); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Scope<AudioSource>, AudioContext* context);
	};
}