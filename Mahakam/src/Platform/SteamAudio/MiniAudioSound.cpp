#include "mhpch.h"
#include "MiniAudioSound.h"

#include "Mahakam/Audio/AudioEngine.h"

namespace Mahakam
{
    //Ref<Sound> Sound::Create(const std::string& filepath, AudioContext* context)
    MH_DEFINE_FUNC(Sound::CreateImpl, Asset<Sound>, const std::string& filepath, const SoundProps& props, AudioContext* context)
    {
        return Asset<MiniAudioSound>::Create(filepath, props, static_cast<MiniAudioContext*>(context));
    };

    MiniAudioSound::MiniAudioSound(const std::string& filepath, const SoundProps& props, MiniAudioContext* context)
        : m_Filepath(filepath), m_Props(props)
	{
        /*
        The binaural node will need to know the input channel count of the sound so we'll need to load
        the sound first. We'll initialize this such that it'll be initially detached from the graph.
        It will be attached to the graph after the binaural node is initialized.
        */
        ma_sound_config soundConfig;

        soundConfig = ma_sound_config_init();
        soundConfig.pFilePath = filepath.c_str();
        soundConfig.isLooping = m_Props.loop ? MA_TRUE : MA_FALSE;
        soundConfig.flags = MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT | MA_SOUND_FLAG_NO_SPATIALIZATION;  /* We'll attach this to the graph later. */

        ma_result result = ma_sound_init_ex(&context->GetEngine(), &soundConfig, &m_Sound);
        MH_CORE_ASSERT(result == MA_SUCCESS, "Failed to initialize sound.");

        /* We'll let the Steam Audio binaural effect do the directional attenuation for us. */
        ma_sound_set_directional_attenuation_factor(&m_Sound, 0);

        ma_sound_set_volume(&m_Sound, m_Props.volume);
	}

	MiniAudioSound::~MiniAudioSound()
	{
        ma_sound_uninit(&m_Sound);
	}
}