#pragma once
#include "../utility/assert.h"
#include "../utility/bits.h"
#include "../utility/meta.h"
#include "../utility/parameter.h"

#include "../stream/serialize_traits.h"

#include <cstdint>
#include <limits>
#include <type_traits>

namespace bitstream
{
	/**
	 * @brief Wrapper type for compiletime known integer bounds
	 * @tparam T 
	*/
	template<typename T, T = (std::numeric_limits<T>::min)(), T = (std::numeric_limits<T>::max)()>
	struct bounded_int;

#pragma region integral types
	/**
	 * @brief A trait used to serialize integer values with runtime bounds
	 * @tparam T A type matching an integer value
	*/
	template<typename T>
	struct serialize_traits<T, typename std::enable_if_t<std::is_integral_v<T> && !std::is_const_v<T>>>
	{
		static_assert(sizeof(T) <= 8, "Integers larger than 8 bytes are currently not supported. You will have to write this functionality yourself");

		/**
		 * @brief Writes an integer into the @p writer
		 * @param writer The stream to write to
		 * @param value The value to serialize
		 * @param min The minimum bound that @p value can be. Inclusive
		 * @param max The maximum bound that @p value can be. Inclusive
		 * @return Success
		*/
		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, in<T> value, T min = (std::numeric_limits<T>::min)(), T max = (std::numeric_limits<T>::max)()) noexcept
		{
			BS_ASSERT(min < max);
            
            BS_ASSERT(value >= min && value <= max);

			uint32_t num_bits = utility::bits_in_range(min, max);

			BS_ASSERT(num_bits <= sizeof(T) * 8);

			if constexpr (sizeof(T) > 4)
			{
				if (num_bits > 32)
				{
					// If the given range is bigger than a word (32 bits)
					uint32_t unsigned_value = static_cast<uint32_t>(value - min);
					BS_ASSERT(writer.serialize_bits(unsigned_value, 32));

					unsigned_value = static_cast<uint32_t>((value - min) >> 32);
					BS_ASSERT(writer.serialize_bits(unsigned_value, num_bits - 32));

					return true;
				}
			}

			// If the given range is smaller than or equal to a word (32 bits)
			uint32_t unsigned_value = static_cast<uint32_t>(value - min);
			BS_ASSERT(writer.serialize_bits(unsigned_value, num_bits));

			return true;
		}

		/**
		 * @brief Reads an integer from the @p reader into @p value
		 * @param reader The stream to read from
		 * @param value The value to read into
		 * @param min The minimum bound that @p value can be. Inclusive
		 * @param max The maximum bound that @p value can be. Inclusive
		 * @return Success
		*/
		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& reader, T& value, T min = (std::numeric_limits<T>::min)(), T max = (std::numeric_limits<T>::max)()) noexcept
		{
			BS_ASSERT(min < max);

			uint32_t num_bits = utility::bits_in_range(min, max);

			BS_ASSERT(num_bits <= sizeof(T) * 8);

			if constexpr (sizeof(T) > 4)
			{
				if (num_bits > 32)
				{
					// If the given range is bigger than a word (32 bits)
					value = 0;
					uint32_t unsigned_value;

					BS_ASSERT(reader.serialize_bits(unsigned_value, 32));
					value |= static_cast<T>(unsigned_value);

					BS_ASSERT(reader.serialize_bits(unsigned_value, num_bits - 32));
					value |= static_cast<T>(unsigned_value) << 32;

					value += min;

					BS_ASSERT(value >= min && value <= max);

					return true;
				}
			}

			// If the given range is smaller than or equal to a word (32 bits)
			uint32_t unsigned_value;
			BS_ASSERT(reader.serialize_bits(unsigned_value, num_bits));

			value = static_cast<T>(unsigned_value) + min;
            
            BS_ASSERT(value >= min && value <= max);

			return true;
		}
	};
#pragma endregion

#pragma region const integral types
	/**
	 * @brief A trait used to serialize integer values with compiletime bounds
	 * @tparam T A type matching an integer value
	 * @tparam Min The lower bound. Inclusive
	 * @tparam Max The upper bound. Inclusive
	*/
	template<typename T, T Min, T Max>
	struct serialize_traits<bounded_int<T, Min, Max>, typename std::enable_if_t<std::is_integral_v<T> && !std::is_const_v<T>>>
	{
		static_assert(sizeof(T) <= 8, "Integers larger than 8 bytes are currently not supported. You will have to write this functionality yourself");

		/**
		 * @brief Writes an integer into the @p writer
		 * @param writer The stream to write to
		 * @param value The value to serialize
		 * @return Success
		*/
		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, in<T> value) noexcept
		{
			static_assert(Min < Max);
            
            BS_ASSERT(value >= Min && value <= Max);

			constexpr uint32_t num_bits = utility::bits_in_range(Min, Max);

			static_assert(num_bits <= sizeof(T) * 8);

			if constexpr (sizeof(T) > 4 && num_bits > 32)
			{
				// If the given range is bigger than a word (32 bits)
				uint32_t unsigned_value = static_cast<uint32_t>(value - Min);
				BS_ASSERT(writer.serialize_bits(unsigned_value, 32));

				unsigned_value = static_cast<uint32_t>((value - Min) >> 32);
				BS_ASSERT(writer.serialize_bits(unsigned_value, num_bits - 32));
			}
			else
			{
				// If the given range is smaller than or equal to a word (32 bits)
				uint32_t unsigned_value = static_cast<uint32_t>(value - Min);
				BS_ASSERT(writer.serialize_bits(unsigned_value, num_bits));
			}

			return true;
		}

		/**
		 * @brief Reads an integer from the @p writer into @p value
		 * @param reader The stream to read from
		 * @param value The value to serialize
		 * @return Success
		*/
		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& reader, T& value) noexcept
		{
			static_assert(Min < Max);

			constexpr uint32_t num_bits = utility::bits_in_range(Min, Max);

			static_assert(num_bits <= sizeof(T) * 8);

			if constexpr (sizeof(T) > 4 && num_bits > 32)
			{
				// If the given range is bigger than a word (32 bits)
				value = 0;
				uint32_t unsigned_value;

				BS_ASSERT(reader.serialize_bits(unsigned_value, 32));
				value |= static_cast<T>(unsigned_value);

				BS_ASSERT(reader.serialize_bits(unsigned_value, num_bits - 32));
				value |= static_cast<T>(unsigned_value) << 32;

				value += Min;
			}
			else
			{
				// If the given range is smaller than or equal to a word (32 bits)
				uint32_t unsigned_value;
				BS_ASSERT(reader.serialize_bits(unsigned_value, num_bits));

				value = static_cast<T>(unsigned_value) + Min;
			}
            
            BS_ASSERT(value >= Min && value <= Max);

			return true;
		}
	};
#pragma endregion
}