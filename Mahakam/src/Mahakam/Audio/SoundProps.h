#pragma once

namespace Mahakam
{
	struct SoundProps
	{
		float Volume;
		bool Loop;

		SoundProps(float volume = 1.0f, bool loop = false)
			: Volume(volume), Loop(loop) {}
	};
}