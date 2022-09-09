#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Core/SharedLibrary.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_float4x4.hpp>

#include <string>

namespace Mahakam
{
	class AudioContext;

	class AudioEngine
	{
	private:
		static AudioContext* m_Context;

	public:
		static void Init();
		static void Shutdown();

		inline static void UpdateSounds(const glm::mat4& listenerTransform) { UpdateSoundsImpl(listenerTransform); }

		inline static AudioContext* GetContext() { return GetContextImpl(); }

	private:
		MH_DECLARE_FUNC(UpdateSoundsImpl, void, const glm::mat4& listenerTransform);

		MH_DECLARE_FUNC(GetContextImpl, AudioContext*);
	};
}