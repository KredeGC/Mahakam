#pragma once

#include "Mahakam/Audio/AudioContext.h"
#include "AudioBuild.h"

#include <vector>

#define MH_AUDIO_SIZE 256

namespace Mahakam
{
	class MiniAudioSource;

	class MiniAudioContext : public AudioContext
	{
	private:
		ma_engine m_Engine;
		ma_engine_config m_EngineConfig;

		IPLContext m_IplContext;

		IPLHRTF m_IPLHRTF;
		IPLAudioSettings m_IplAudioSettings;

		std::vector<MiniAudioSource*> m_Sources;

	public:
		MiniAudioContext();
		~MiniAudioContext();

		virtual void UpdateSounds(const glm::mat4& listenerTransform) override;

		void AddSource(MiniAudioSource* node) { m_Sources.push_back(node); }
		void RemoveSource(MiniAudioSource* node);

		ma_engine& GetEngine() { return m_Engine; }
		ma_engine_config& GetEngineConfig() { return m_EngineConfig; }
		IPLContext& GetIPLContext() { return m_IplContext; }

		IPLHRTF& GetHRTF() { return m_IPLHRTF; }
		IPLAudioSettings& GetAudioSettings() { return m_IplAudioSettings; }
	};
}