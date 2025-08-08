#pragma once

#include "Mahakam/Audio/Sound.h"
#include "MiniAudioContext.h"

namespace Mahakam
{
	class MiniAudioSound : public Sound
	{
	private:
		std::string m_Filepath;

	public:
		MiniAudioSound(const std::string& filepath, MiniAudioContext* context);
		~MiniAudioSound() = default;

		virtual const std::string& GetFilepath() const override { return m_Filepath; }
	};
}