#pragma once

#include "AudioEngine.h"
#include "AudioContext.h"

namespace Mahakam
{
	class Sound
	{
	public:
		virtual const std::string& GetFilePath() const = 0;

		inline static Ref<Sound> Create(const std::string& filepath) { return CreateImpl(filepath, AudioEngine::GetContext()); }
		inline static Ref<Sound> Create(const std::string& filepath, AudioContext* context) { return CreateImpl(filepath, context); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Ref<Sound>, const std::string& filepath, AudioContext* context);
	};
}