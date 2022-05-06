#pragma once

#include <glm/glm.hpp>

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