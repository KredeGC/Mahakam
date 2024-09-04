#pragma once

#include "MiniAudioContext.h"

#include "Mahakam/Audio/AudioDataSource.h"

namespace Mahakam
{
	class MiniAudioDataSource : public AudioDataSource
	{
	public:
		MiniAudioDataSource(Asset<Sound> soundAsset, MiniAudioContext* context);
		virtual ~MiniAudioDataSource();

		virtual void* GetDataSource() override { return &m_DataSource; }

	private:
		Asset<Sound> m_Sound;
		ma_resource_manager_data_source m_DataSource;
	};
}