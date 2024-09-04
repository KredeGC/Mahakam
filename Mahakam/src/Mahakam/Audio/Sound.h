#pragma once

#include "AudioEngine.h"

#include "Mahakam/Asset/Asset.h"

namespace Mahakam
{
	class Sound
	{
	public:
		virtual const std::string& GetFilepath() const = 0;

		inline static Asset<Sound> Create(const std::string& filepath) { return CreateImpl(filepath, AudioEngine::GetContext()); }
		inline static Asset<Sound> Create(const std::string& filepath, AudioContext* context) { return CreateImpl(filepath, context); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Asset<Sound>, const std::string& filepath, AudioContext* context);
	};
}