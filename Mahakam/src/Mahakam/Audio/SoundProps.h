#pragma once

#include "Mahakam/Core/Core.h"

namespace Mahakam
{
	struct SoundProps
	{
		bool loop;
		float volume;

		SoundProps(float volume = 1.0f, bool loop = false)
			: volume(volume), loop(loop) {}
	};
}