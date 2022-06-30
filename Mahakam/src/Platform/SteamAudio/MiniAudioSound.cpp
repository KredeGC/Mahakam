#include "mhpch.h"
#include "MiniAudioSound.h"

#include "Mahakam/Audio/AudioEngine.h"

namespace Mahakam
{
    //Ref<Sound> Sound::Create(const std::string& filepath, AudioContext* context)
    MH_DEFINE_FUNC(Sound::CreateImpl, Asset<Sound>, const std::string& filepath, const SoundProps& props, AudioContext* context)
    {
        return Asset<MiniAudioSound>(CreateRef<MiniAudioSound>(filepath, props, static_cast<MiniAudioContext*>(context)));
    };

    MiniAudioSound::MiniAudioSound(const std::string& filepath, const SoundProps& props, MiniAudioContext* context)
        : m_Filepath(filepath), m_Props(props)
	{ }

    void MiniAudioSound::SetProps(const SoundProps& props)
    {
        m_Props = props;
    }
}