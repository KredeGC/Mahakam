#pragma once

#include "Mahakam/Asset/Asset.h"
#include "AudioEngine.h"
#include "AudioContext.h"
#include "SoundProps.h"

namespace Mahakam
{
	class Sound
	{
	public:
		virtual const std::string& GetFilepath() const = 0;

		virtual const SoundProps& GetProps() const = 0;

		inline static Asset<Sound> Create(const std::string& filepath, const SoundProps& props = {}) { return CreateImpl(filepath, props, AudioEngine::GetContext()); }
		inline static Asset<Sound> Create(const std::string& filepath, AudioContext* context, const SoundProps& props = {}) { return CreateImpl(filepath, props, context); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<Sound>, const std::string& filepath, const SoundProps& props, AudioContext* context);
	};
}