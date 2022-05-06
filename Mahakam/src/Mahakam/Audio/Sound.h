#pragma once

#include "AudioContext.h"

namespace Mahakam
{
	class Sound
	{
	public:
		virtual const std::string& GetFilePath() const = 0;

		static Ref<Sound> Create(const std::string& filepath, AudioContext* context);
	};
}