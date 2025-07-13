#pragma once

#include "AssetSerializeTraits.h"

#include "Mahakam/Renderer/Animation.h"

#include "Mahakam/BinarySerialization/ContainerSerialization.h"
#include "Mahakam/BinarySerialization/MathSerialization.h"
#include "Mahakam/BinarySerialization/StringSerialization.h"

namespace Mahakam::Serialization
{
	template<>
	struct SerializeTraits<Animation::Sampler>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Animation::Sampler> sampler) noexcept
		{
			return stream.serialize(sampler.Timestamps)
				&& stream.serialize(sampler.Translations)
				&& stream.serialize(sampler.Rotations)
				&& stream.serialize(sampler.Scales);
		}
	};

	template<>
	struct AssetSerializeTraits<Animation>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Asset<Animation>> animation) noexcept
		{
			if constexpr (Stream::writing)
			{
				return stream.serialize(animation->GetName())
					&& stream.serialize(animation->GetDuration())
					&& stream.serialize(animation->GetSamplers());
			}
			else
			{
				std::string name;
				float duration;
				Animation::SamplerType samplers{ Allocator::GetAllocator<Animation::Sampler>() };

				if (!stream.serialize(name)
					|| !stream.serialize(duration)
					|| !stream.serialize(samplers))
					return false;

				animation = Animation::Create(name, std::move(samplers), duration);
			}

			return true;
		}
	};
}