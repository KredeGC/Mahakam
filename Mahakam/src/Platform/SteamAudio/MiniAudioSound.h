#pragma once

#include "Mahakam/Audio/Sound.h"
#include "MiniAudioContext.h"

namespace Mahakam
{
	class MiniAudioSound : public Sound
	{
	private:
		std::string m_Filepath;
		SoundProps m_Props;
		//ma_sound m_Sound;

	public:
		MiniAudioSound(const std::string& filepath, const SoundProps& props, MiniAudioContext* context);
		~MiniAudioSound();

		virtual const std::string& GetFilepath() const override { return m_Filepath; }

		virtual const SoundProps& GetProps() const override { return m_Props; }

		//ma_sound& GetNativeSound() { return m_Sound; }
	};
}