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

		/* Set the spatial blend to full spatialization. */
		m_Node.spatialBlend = 1.0f;
		m_Node.interpolate = false;

		/* Connect the output of the delay node to the input of the endpoint. */
		ma_node_attach_output_bus(&m_Node, 0, ma_engine_get_endpoint(&m_Context->GetEngine()), 0);

		m_Context->AddSource(this);
	}

	MiniAudioSource::~MiniAudioSource()
	{
		m_Context->RemoveSource(this);

		if (m_Sound)
			UninitSound();

		ma_steamaudio_binaural_node_uninit(&m_Node, NULL);
	}

	void MiniAudioSource::Play()
	{
		if (m_Sound)
			ma_sound_start(&m_MaSound);
	}

	void MiniAudioSource::Stop()
	{
		if (m_Sound)
			ma_sound_stop(&m_MaSound);
	}

	void MiniAudioSource::SetSound(Asset<Sound> sound)
	{
		if (m_Sound)
			UninitSound();

		m_Sound = static_cast<Asset<MiniAudioSound>>(sound);
		m_SoundProps = m_Sound->GetProps();
		m_SoundSwitch = m_Sound.Get();

		InitSound();
	}

	void MiniAudioSource::SetInterpolation(bool interpolate)
	{
		m_Node.interpolate = interpolate;
	}

	void MiniAudioSource::SetSpatialBlend(float blend)
	{
		m_Node.spatialBlend = blend;
	}

	void MiniAudioSource::SetPosition(const glm::vec3& source)
	{
		m_Source = { source, 1.0f };
	}

	void MiniAudioSource::UpdatePosition(const glm::mat4& listenerView, const glm::vec3& listenerPos)
	{
		const SoundProps& props = m_Sound->GetProps();

		// Reload the sound if the path changed
		if (m_Sound.Get().get() != m_SoundSwitch.get())
		{
			UninitSound();

			m_SoundProps = props;
			m_SoundSwitch = m_Sound.Get();

			InitSound();

			Play(); // TEMP
		}

		// Update volume, if it has changed
		if (props.volume != m_SoundProps.volume)
		{
			m_SoundProps.volume = props.volume;
			ma_sound_set_volume(&m_MaSound, props.volume);
		}

		// Update looping, if it has changed
		if (props.loop != m_SoundProps.loop)
		{
			m_SoundProps.loop = props.loop;
			ma_sound_set_looping(&m_MaSound, props.loop ? MA_TRUE : MA_FALSE);
		}

		if (m_Node.spatialBlend > 0.0f)
		{
			glm::vec3 direction = listenerView * m_Source;

			IPLVector3 iplDirection = { direction.x, direction.y, direction.z };
			IPLVector3 iplListener = { listenerPos.x, listenerPos.y, listenerPos.z };
			IPLVector3 iplSource = { m_Source.x, m_Source.y, m_Source.z };

			ma_steamaudio_binaural_node_set_position(&m_Node, iplDirection, iplSource, iplListener);
		}
	}

	void MiniAudioSource::InitSound()
	{
		/*
		The binaural node will need to know the input channel count of the sound so we'll need to load
		the sound first. We'll initialize this such that it'll be initially detached from the graph.
		It will be attached to the graph after the binaural node is initialized.
		*/
		ma_sound_config soundConfig;

		std::string filepath = m_Sound->GetFilepath();

		soundConfig = ma_sound_config_init();
		soundConfig.pFilePath = filepath.c_str();
		soundConfig.isLooping = m_Sound->GetProps().loop ? MA_TRUE : MA_FALSE;
		soundConfig.flags = MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT | MA_SOUND_FLAG_NO_SPATIALIZATION;  /* We'll attach this to the graph later. */

		ma_result result = ma_sound_init_ex(&m_Context->GetEngine(), &soundConfig, &m_MaSound);
		MH_CORE_ASSERT(result == MA_SUCCESS, "Failed to initialize sound.");

		/* We'll let the Steam Audio binaural effect do the directional attenuation for us. */
		ma_sound_set_directional_attenuation_factor(&m_MaSound, 0);

		ma_sound_set_volume(&m_MaSound, m_Sound->GetProps().volume);

		/* We can now wire up the sound to the binaural node and start it. */
		ma_node_attach_output_bus(&m_MaSound, 0, &m_Node, 0);
	}

	void MiniAudioSource::UninitSound()
	{
		ma_sound_stop(&m_MaSound);
		ma_node_detach_output_bus(&m_MaSound, 0);
		ma_sound_uninit(&m_MaSound);
	}
}