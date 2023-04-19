#pragma once

#include "Mahakam/Core/Types.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_float4x4.hpp>

namespace Mahakam
{
	class AudioContext
	{
	public:
		virtual ~AudioContext() {}

		virtual void UpdateSounds(const glm::mat4& listenerTransform) = 0;

		static Scope<AudioContext> Create();
	};
}