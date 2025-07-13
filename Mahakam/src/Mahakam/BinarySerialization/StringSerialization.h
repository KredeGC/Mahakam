#pragma once

#include "SerializeTraits.h"

#include "IntegralSerialization.h"

#include <string>

namespace Mahakam::Serialization
{
	template<>
	struct SerializeTraits<std::string>
	{
		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, const std::string& value)
		{
			uint32_t length = static_cast<uint32_t>(value.size());

			MH_SER_ASSERT(writer.serialize(length));

			return writer.serialize_bytes(value.data(), length);
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& reader, std::string& value)
		{
			uint32_t length;
			MH_SER_ASSERT(reader.serialize(length));

			if (length == 0)
			{
				value.clear();
				return true;
			}

			value.resize(length);

			return reader.serialize_bytes(value.data(), length);
		}
	};
}