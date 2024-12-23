#pragma once

#include "Mahakam/Renderer/Animation.h"

#include "ContainerSerialization.h"
#include "MathSerialization.h"

#include <bitstream.h>

namespace bitstream
{
	template<>
	struct serialize_traits<Mahakam::Animation::Sampler>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Mahakam::Animation::Sampler> sampler) noexcept
		{
			BS_ASSERT(stream.serialize(sampler.Timestamps));
			BS_ASSERT(stream.serialize(sampler.Translations));
			BS_ASSERT(stream.serialize(sampler.Rotations));
			BS_ASSERT(stream.serialize(sampler.Scales));

			return true;
		}
	};

	template<>
	struct serialize_traits<Mahakam::Animation>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Mahakam::Asset<Mahakam::Animation>> animation) noexcept
		{
			using namespace Mahakam;

			if constexpr (Stream::writing)
			{
				BS_ASSERT(stream.serialize(animation->GetName(), 256));
				BS_ASSERT(stream.serialize(animation->GetDuration()));
				BS_ASSERT(stream.serialize(animation->GetSamplers()));
			}
			else
			{
				std::string name;
				BS_ASSERT(stream.serialize(name, 256));

				float duration;
				BS_ASSERT(stream.serialize(duration));

				Animation::SamplerType samplers{ Allocator::GetAllocator<Animation::Sampler>() };
				BS_ASSERT(stream.serialize(samplers));

				animation = Animation::Create(name, std::move(samplers), duration);
			}

			return true;
		}
	};
}