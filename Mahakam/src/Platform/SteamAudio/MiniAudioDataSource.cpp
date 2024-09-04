#include "Mahakam/mhpch.h"

#include "MiniAudioDataSource.h"

#include "Mahakam/Audio/AudioDataSource.h"

namespace Mahakam
{
	Scope<AudioDataSource> AudioDataSource::Create(Asset<Sound> soundAsset, AudioContext* context)
	{
		return CreateScope<MiniAudioDataSource>(soundAsset, static_cast<MiniAudioContext*>(context));
	};

	MiniAudioDataSource::MiniAudioDataSource(Asset<Sound> soundAsset, MiniAudioContext* context) :
		m_Sound(std::move(soundAsset))
	{
		std::string filepath = m_Sound->GetFilepath();

		auto* resourceManager = static_cast<MiniAudioContext*>(context)->GetEngine().pResourceManager;

		ma_result result = ma_resource_manager_data_source_init(resourceManager, filepath.c_str(), 0, NULL, &m_DataSource);
		MH_ASSERT(result == MA_SUCCESS, "Failed to initialize data source");

		ma_data_source_set_looping(&m_DataSource, m_Sound->GetProps().Loop);
	}

	MiniAudioDataSource::~MiniAudioDataSource()
	{
		ma_resource_manager_data_source_uninit(&m_DataSource);
	}
}