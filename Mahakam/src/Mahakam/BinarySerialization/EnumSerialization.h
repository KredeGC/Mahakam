#pragma once

#include "SerializeTraits.h"

#include "MathSerialization.h"

namespace Mahakam::Serialization
{
	template<typename T>
	struct SerializeTraits<T, typename std::enable_if_t<std::is_enum_v<T> && !std::is_const_v<T>>>
	{
		using value_type = std::underlying_type_t<T>;

		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, T value) noexcept
		{
			value_type unsigned_value = static_cast<value_type>(value);

			return writer.serialize(unsigned_value);
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& reader, T& value) noexcept
		{
			value_type unsigned_value;

			MH_SER_ASSERT(reader.serialize(unsigned_value));

			value = static_cast<T>(unsigned_value);

			return true;
		}
	};
}