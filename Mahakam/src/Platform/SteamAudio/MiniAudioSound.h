#pragma once

#include "Mahakam/Audio/Sound.h"
#include "MiniAudioContext.h"

namespace Mahakam
{
	class MiniAudioSound : public Sound
	{
	private:
		std::string filepath;
		ma_sound m_Sound;

	public:
		MiniAudioSound(const std::string& filepath, MiniAudioContext* context, bool loop = false);
		~MiniAudioSound();

		virtual const std::string& GetFilePath() const override { return filepath; }

		ma_sound& GetNativeSound() { return m_Sound; } // Unused
	};
}