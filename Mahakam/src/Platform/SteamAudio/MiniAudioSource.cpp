#include "mhpch.h"
#include "MiniAudioSource.h"

namespace Mahakam
{
    //Ref<AudioSource> AudioSource::Create(AudioContext* context)
    MH_DEFINE_FUNC(AudioSource::CreateImpl, Ref<AudioSource>, AudioContext* context)
    {
        return CreateRef<MiniAudioSource>(static_cast<MiniAudioContext*>(context));
    };

    MiniAudioSource::MiniAudioSource(MiniAudioContext* context)
        : m_Context(context)
	{
        ma_steamaudio_binaural_node_config binauralNodeConfig;

        /*
        For this example we're just using the engine's channel count, but a more optimal solution
        might be to set this to mono if the source data is also mono.
        */
        binauralNodeConfig = ma_steamaudio_binaural_node_config_init(m_Context->GetEngineConfig().channels, m_Context->GetAudioSettings(), m_Context->GetIPLContext(), m_Context->GetHRTF());

        ma_result result = ma_steamaudio_binaural_node_init(ma_engine_get_node_graph(&m_Context->GetEngine()), &binauralNodeConfig, NULL, &m_Node);
        MH_CORE_ASSERT(result == MA_SUCCESS, "Failed to initialize binaural node.");

        /* Connect the output of the delay node to the input of the endpoint. */
        ma_node_attach_output_bus(&m_Node, 0, ma_engine_get_endpoint(&m_Context->GetEngine()), 0);

        m_Context->AddSource(this);
	}

	MiniAudioSource::~MiniAudioSource()
	{
        m_Context->RemoveSource(this);

        ma_steamaudio_binaural_node_uninit(&m_Node, NULL);
	}

    void MiniAudioSource::Play()
    {
        ma_sound_start(&m_Sound->GetNativeSound());
    }

    void MiniAudioSource::SetSound(Ref<Sound> sound)
    {
        if (m_Sound)
            ma_node_detach_output_bus(&m_Sound->GetNativeSound(), 0);

        m_Sound = std::static_pointer_cast<MiniAudioSound>(sound);

        /* We can now wire up the sound to the binaural node and start it. */
        ma_node_attach_output_bus(&m_Sound->GetNativeSound(), 0, &m_Node, 0);
    }

    void MiniAudioSource::SetPosition(const glm::vec3& source)
    {
        m_Source = { source, 1.0f };
    }

    void MiniAudioSource::UpdatePosition(const glm::mat4& listenerView, const glm::vec3& listenerPos)
    {
        glm::vec3 direction = listenerView * m_Source;

        IPLVector3 iplDirection = { direction.x, direction.y, direction.z };
        IPLVector3 iplListener = { listenerPos.x, listenerPos.y, listenerPos.z };
        IPLVector3 iplSource = { m_Source.x, m_Source.y, m_Source.z };

        ma_steamaudio_binaural_node_set_position(&m_Node, iplDirection, iplSource, iplListener);
    }
}