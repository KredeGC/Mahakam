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

		Ref<MiniAudioSound> m_Sound;

		glm::vec4 m_Source{ 0 };

	public:
		MiniAudioSource(MiniAudioContext* context);
		~MiniAudioSource();

		virtual void Play() override;
		virtual void Stop() override;

		virtual void SetSound(Ref<Sound> sound) override;
		virtual Ref<Sound> GetSound() override { return m_Sound; }

		virtual void SetSpatialBlend(float blend) override;
		virtual float GetSpatialBlend() override { return m_Node.spatialBlend; }

		virtual void SetPosition(const glm::vec3& source) override;

		void UpdatePosition(const glm::mat4& listenerView, const glm::vec3& listenerPos);

		ma_steamaudio_binaural_node& GetNode() { return m_Node; }
	};
}