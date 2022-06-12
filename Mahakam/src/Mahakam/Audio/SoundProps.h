#pragma once

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