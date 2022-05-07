#pragma once

#include "Mahakam/Core/Core.h"

namespace Mahakam
{
	struct SoundProps
	{
		float volume;
		bool loop;

		SoundProps(float volume = 1.0f, bool loop = false)
			: volume(volume), loop(loop) {}
	};
}