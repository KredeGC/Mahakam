#pragma once

#include "Mahakam/Audio/AudioSource.h"
#include "Mahakam/Audio/AudioDataSource.h"

#include "MiniAudioContext.h"
#include "MiniAudioSound.h"

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace Mahakam
{
	class MiniAudioSource : public AudioSource
	{
	private:
		ma_steamaudio_binaural_node m_Node;

		MiniAudioContext* m_Context;

		// ma_sound is internally ref-counted, so this doesn't matter performance-wise
		ma_sound m_MaSound;

		// Need to keep a reference to data source
		Scope<AudioDataSource> m_DataSource;

		SoundProps m_Props;

		glm::vec4 m_Source{ 0 };

	public:
		MiniAudioSource(MiniAudioContext* context);
		~MiniAudioSource();

		virtual void Play() override;
		virtual void Stop() override;

		virtual void SetDataSource(Scope<AudioDataSource> dataSource) override;
		virtual AudioDataSource* GetDataSource() const override { return m_DataSource.get(); }

		virtual const SoundProps& GetProps() const override { return m_Props; }
		virtual void SetProps(const SoundProps& props) override;

		virtual void SetVolume(float volume) override;
		virtual float GetVolume() const override { return m_Props.Volume; }

		virtual void SetLooping(bool loop) override;
		virtual bool GetLooping() const override { return m_Props.Loop; }

		virtual void SetInterpolation(bool interpolate) override;
		virtual bool GetInterpolation() const override { return m_Node.interpolate; }

		virtual void SetSpatialBlend(float blend) override;
		virtual float GetSpatialBlend() const override { return m_Node.spatialBlend; }

		virtual float GetTime() const override;
		virtual float GetDuration() const override;

		virtual void SetPosition(const glm::vec3& source) override;

		virtual AudioContext* GetContext() const override { return m_Context; }

		void UpdatePosition(const glm::mat4& listenerView, const glm::vec3& listenerPos);

		ma_steamaudio_binaural_node& GetNode() { return m_Node; }

	private:
		void InitSound();

		void UninitSound();
	};
}