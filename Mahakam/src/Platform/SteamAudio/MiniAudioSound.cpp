#include "Mahakam/mhpch.h"
#include "MiniAudioSound.h"

#include "Mahakam/Audio/AudioEngine.h"

namespace Mahakam
{
    //Asset<Sound> Sound::Create(const std::string& filepath, AudioContext* context)
    MH_DEFINE_FUNC(Sound::CreateImpl, Asset<Sound>, const std::string& filepath, const SoundProps& props, AudioContext* context)
    {
        return CreateAsset<MiniAudioSound>(filepath, props, static_cast<MiniAudioContext*>(context));
    };

    MiniAudioSound::MiniAudioSound(const std::string& filepath, const SoundProps& props, MiniAudioContext* context)
        : m_Filepath(filepath), m_Props(props)
	{
        // TODO: Use stb_vorbis as internal format?
        // TODO: Read file as binary (wav, mp3)

        // Decode the file
        //ma_decoder decoder;
        //ma_decoder_init_memory(nullptr, 0, nullptr, &decoder);

        // Read the length of the decoded file
        //uint64_t length;
        //ma_decoder_get_length_in_pcm_frames(&decoder, &length);

        // Read the PCM frames back into a buffer
        //void* frames;
        //ma_result result = ma_decoder_read_pcm_frames(&decoder, frames, framesToRead, &framesRead);
        //if (framesRead < framesToRead) {
        //    // Reached the end.
        //}
    }

    void MiniAudioSound::SetProps(const SoundProps& props)
    {
        m_Props = props;
    }
}