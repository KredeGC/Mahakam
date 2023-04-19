#pragma once

#include <array>
#include <cstdint>

namespace bitstream::utility
{
	inline constexpr auto CHECKSUM_TABLE = []()
	{
		constexpr uint32_t POLYNOMIAL = 0xEDB88320;

		std::array<uint32_t, 0x100> table{};

		for (uint32_t i = 0; i < 0x100; ++i)
		{
			uint32_t item = i;
			for (uint32_t bit = 0; bit < 8; ++bit)
				item = ((item & 1) != 0) ? (POLYNOMIAL ^ (item >> 1)) : (item >> 1);
			table[i] = item;
		}

		return table;
	}();

	inline constexpr uint32_t crc_uint32(const uint8_t* bytes, uint32_t size)
	{
		uint32_t result = 0xFFFFFFFF;

		for (uint32_t i = 0; i < size; i++)
			result = CHECKSUM_TABLE[(result & 0xFF) ^ *(bytes + i)] ^ (result >> 8);

		return ~result;
	}
}