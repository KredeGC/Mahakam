#pragma once

#include <glm/ext/matrix_float4x4.hpp>

namespace Mahakam
{
	class AudioContext
	{
	public:
		virtual ~AudioContext() {}

		virtual void UpdateSounds(const glm::mat4& listenerTransform) = 0;

		static AudioContext* Create();
	};
}