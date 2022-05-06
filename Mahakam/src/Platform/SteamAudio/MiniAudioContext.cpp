#include "mhpch.h"
#include "MiniAudioContext.h"
#include "MiniAudioSource.h"

#include <algorithm>

namespace Mahakam
{
	AudioContext* AudioContext::Create()
	{
		return new MiniAudioContext();
	}

	MiniAudioContext::MiniAudioContext()
	{
		ma_result result;

		/* The engine needs to be initialized first. */
		m_EngineConfig = ma_engine_config_init();
		m_EngineConfig.channels = 2;
		m_EngineConfig.sampleRate = 48000;
		m_EngineConfig.periodSizeInFrames = MH_AUDIO_SIZE;

		result = ma_engine_init(&m_EngineConfig, &m_Engine);
		MH_CORE_ASSERT(result == MA_SUCCESS, "Failed to initialize audio engine.");

		IPLContextSettings iplContextSettings;

		/* IPLContext */
		memset(((&iplContextSettings)), 0, (sizeof(*(&iplContextSettings))));
		iplContextSettings.version = STEAMAUDIO_VERSION;

		result = ma_result_from_IPLerror(iplContextCreate(&iplContextSettings, &m_IplContext));
		MH_CORE_ASSERT(result == MA_SUCCESS, "Failed to initialize audio context.");

		IPLHRTFSettings iplHRTFSettings;

		/*
		Now that we have the engine we can initialize the Steam Audio objects.
		*/
		memset(((&m_IplAudioSettings)), 0, (sizeof(*(&m_IplAudioSettings))));
		m_IplAudioSettings.samplingRate = ma_engine_get_sample_rate(&m_Engine);

		/*
		If there's any Steam Audio developers reading this, why is the frame size needed? This needs to
		be documented. If this is for some kind of buffer management with FFT or something, then this
		need not be exposed to the public API. There should be no need for the public API to require a
		fixed sized update.
		*/
		m_IplAudioSettings.frameSize = MH_AUDIO_SIZE;

		/* IPLHRTF */
		memset(((&iplHRTFSettings)), 0, (sizeof(*(&iplHRTFSettings))));
		iplHRTFSettings.type = IPL_HRTFTYPE_DEFAULT;

		result = ma_result_from_IPLerror(iplHRTFCreate(m_IplContext, &m_IplAudioSettings, &iplHRTFSettings, &m_IPLHRTF));
		MH_CORE_ASSERT(result == MA_SUCCESS, "Failed to initialize audio HRTF.");
	}

	MiniAudioContext::~MiniAudioContext()
	{
		iplHRTFRelease(&m_IPLHRTF);

		iplContextRelease(&m_IplContext);

		ma_engine_uninit(&m_Engine);
	}

	void MiniAudioContext::UpdateSounds(const glm::vec3& listener)
	{
		for (auto& source : m_Sources)
			source->UpdatePosition(listener);
	}

	void MiniAudioContext::RemoveSource(MiniAudioSource* node)
	{
		auto iter = std::find(m_Sources.begin(), m_Sources.end(), node);
		if (iter != m_Sources.end())
			m_Sources.erase(iter);
	}
}