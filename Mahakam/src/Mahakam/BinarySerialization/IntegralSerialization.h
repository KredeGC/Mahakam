#pragma once

#include "SerializeTraits.h"

namespace Mahakam::Serialization
{
	template<typename T>
	struct SerializeTraits<T, typename std::enable_if_t<std::is_integral_v<T> && !std::is_const_v<T>>>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, T> value)
		{
			return stream.serialize_bytes(&value, 1);
		}
	};
}