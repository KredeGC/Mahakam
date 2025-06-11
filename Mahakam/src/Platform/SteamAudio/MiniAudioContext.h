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
	public:
		friend MiniAudioSource;

	private:
		ma_engine m_Engine;
		ma_engine_config m_EngineConfig;

		IPLContext m_IplContext;

		IPLHRTF m_IPLHRTF;
		IPLAudioSettings m_IplAudioSettings;

		TrivialVector<MiniAudioSource*> m_Sources;
		//UnorderedMap<std::string, void*> m_SoundGroups;

		float m_Volume;

	public:
		MiniAudioContext();
		~MiniAudioContext();

		virtual uint32_t GetChannels() const override;
		virtual uint32_t GetSampleRate() const override;
		virtual float GetVolume() const override;

		virtual void SetVolume(float volume) override;

		virtual void UpdateSounds(const glm::mat4& listenerTransform) override;

		ma_engine& GetEngine() { return m_Engine; }
		ma_engine_config& GetEngineConfig() { return m_EngineConfig; }
		IPLContext& GetIPLContext() { return m_IplContext; }

		IPLHRTF& GetHRTF() { return m_IPLHRTF; }
		IPLAudioSettings& GetAudioSettings() { return m_IplAudioSettings; }

	private:
		void AddSource(MiniAudioSource* node) { m_Sources.push_back(node); }
		void RemoveSource(MiniAudioSource* node);
	};
}