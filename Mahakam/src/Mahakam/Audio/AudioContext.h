#pragma once

#include <glm/glm.hpp>

namespace Mahakam
{
	class AudioContext
	{
	public:
		virtual ~AudioContext() {}

		virtual void UpdateSounds(const glm::vec3& listener) = 0;

		static AudioContext* Create();
	};
}