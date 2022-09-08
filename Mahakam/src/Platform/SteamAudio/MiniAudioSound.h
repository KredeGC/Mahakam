#pragma once

#include "Mahakam/Audio/Sound.h"
#include "MiniAudioContext.h"

namespace Mahakam
{
	class MiniAudioSound;

	extern template class Asset<MiniAudioSound>;

	class MiniAudioSound : public Sound
	{
	private:
		std::string m_Filepath;
		SoundProps m_Props;

	public:
		MiniAudioSound(const std::string& filepath, const SoundProps& props, MiniAudioContext* context);
		~MiniAudioSound() = default;

		virtual const std::string& GetFilepath() const override { return m_Filepath; }

		virtual const SoundProps& GetProps() const override { return m_Props; }

		virtual void SetProps(const SoundProps& props) override;
	};
}