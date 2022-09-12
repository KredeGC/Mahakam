#pragma once

#include "AudioEngine.h"
#include "SoundProps.h"

#include "Mahakam/Asset/Asset.h"

namespace Mahakam
{
	class Sound;

	extern template class Asset<Sound>;

	class Sound
	{
	public:
		virtual const std::string& GetFilepath() const = 0;

		virtual const SoundProps& GetProps() const = 0;

		virtual void SetProps(const SoundProps& props) = 0;

		inline static Ref<Sound> Create(const std::string& filepath, const SoundProps& props = {}) { return CreateImpl(filepath, props, AudioEngine::GetContext()); }
		inline static Ref<Sound> Create(const std::string& filepath, AudioContext* context, const SoundProps& props = {}) { return CreateImpl(filepath, props, context); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Ref<Sound>, const std::string& filepath, const SoundProps& props, AudioContext* context);
	};
}