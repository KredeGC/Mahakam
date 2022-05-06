#pragma once

#include "Mahakam/Audio/AudioSource.h"

#include "MiniAudioContext.h"
#include "MiniAudioSound.h"

namespace Mahakam
{
	class MiniAudioSource : public AudioSource
	{
	private:
		ma_steamaudio_binaural_node m_Node;

		MiniAudioContext* m_Context;

		float m_Source[3]{ 0 };

		Ref<MiniAudioSound> m_Sound;

	public:
		MiniAudioSource(MiniAudioContext* context);
		~MiniAudioSource();

		virtual void Play() override;

		virtual void SetSound(Ref<Sound> sound) override;
		virtual Ref<Sound> GetSound() override { return m_Sound; }

		virtual void SetPosition(const glm::vec3& source) override;

		void UpdatePosition(const glm::vec3& listener);

		// These must be called from MiniAudioContext::Update
		//void SetListenerPosition(float x, float y, float z);
		//void SetSourcePosition(float x, float y, float z);

		ma_steamaudio_binaural_node& GetNode() { return m_Node; }
	};
}