#pragma once

#include "Mahakam/Core/Types.h"

#include <bitstream.h>

#include <vector>

namespace bitstream
{
	template<typename T, typename Alloc>
	struct serialize_traits<Mahakam::TrivialArray<T, Alloc>>
	{
		using Container = Mahakam::TrivialArray<T, Alloc>;

		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Container> container) noexcept
		{
			size_t size = container.size();
			BS_ASSERT(stream.template serialize<size_t>(size));

			if constexpr (Stream::reading)
				container.resize(size);

			for (size_t i = 0; i < size; ++i)
			{
				BS_ASSERT(stream.template serialize<T>(container[i]));
			}

			return true;
		}
	};

	template<typename T, typename Alloc>
	struct serialize_traits<Mahakam::TrivialVector<T, Alloc>>
	{
		using Container = Mahakam::TrivialVector<T, Alloc>;

		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Container> container) noexcept
		{
			size_t size = container.size();
			BS_ASSERT(stream.template serialize<size_t>(size));

			if constexpr (Stream::reading)
				container.resize(size);

			for (size_t i = 0; i < size; ++i)
			{
				BS_ASSERT(stream.template serialize<T>(container[i]));
			}

			return true;
		}
	};

	template<typename T, typename Alloc>
	struct serialize_traits<std::vector<T, Alloc>>
	{
		using Container = std::vector<T, Alloc>;

		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Container> container) noexcept
		{
			size_t size = container.size();
			BS_ASSERT(stream.template serialize<size_t>(size));

			if constexpr (Stream::reading)
				container.resize(size);

			for (size_t i = 0; i < size; ++i)
			{
				BS_ASSERT(stream.template serialize<T>(container[i]));
			}

			return true;
		}
	};

	template<typename K, typename V, typename Alloc>
	struct serialize_traits<Mahakam::UnorderedMap<K, V, Alloc>>
	{
		using Container = Mahakam::UnorderedMap<K, V, Alloc>;

		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& stream, const Container& container) noexcept
		{
			BS_ASSERT(stream.serialize(container.size()));

			for (auto& [k, v] : container)
			{
				BS_ASSERT(stream.serialize(k));
				BS_ASSERT(stream.serialize(v));
			}

			return true;
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& stream, Container& container) noexcept
		{
			size_t size = container.size();
			BS_ASSERT(stream.serialize(size));

			if constexpr (Stream::reading)
				container.reserve(size);

			for (size_t i = 0; i < size; ++i)
			{
				K k;
				V v;

				BS_ASSERT(stream.serialize(k));
				BS_ASSERT(stream.serialize(v));

				container.emplace(k, v);
			}

			return true;
		}
	};
}