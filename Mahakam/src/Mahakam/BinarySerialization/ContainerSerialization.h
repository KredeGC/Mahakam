#pragma once

#include "SerializeTraits.h"

#include "Mahakam/Core/Types.h"

#include <vector>

namespace Mahakam::Serialization
{
	template<typename Stream, typename T>
	bool SerializeContainer(Stream& stream, inout<Stream, T> container) noexcept
	{
		auto size = container.size();
		MH_SER_ASSERT(stream.serialize(size));

		if constexpr (Stream::reading)
			container.resize(size);

		for (decltype(size) i = 0; i < size; ++i)
		{
			MH_SER_ASSERT(stream.serialize(container[i]));
		}

		return true;
	}

	template<typename T, typename Alloc>
	struct SerializeTraits<TrivialArray<T, Alloc>>
	{
		using Container = TrivialArray<T, Alloc>;

		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Container> container) noexcept
		{
			return SerializeContainer<Stream, Container>(stream, container);
		}
	};

	template<typename T, typename Alloc>
	struct SerializeTraits<Mahakam::TrivialVector<T, Alloc>>
	{
		using Container = Mahakam::TrivialVector<T, Alloc>;

		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Container> container) noexcept
		{
			return SerializeContainer<Stream, Container>(stream, container);
		}
	};

	template<typename T, typename Alloc>
	struct SerializeTraits<std::vector<T, Alloc>>
	{
		using Container = std::vector<T, Alloc>;

		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Container> container) noexcept
		{
			return SerializeContainer<Stream, Container>(stream, container);
		}
	};

	template<typename K, typename V, typename Alloc>
	struct SerializeTraits<UnorderedMap<K, V, Alloc>>
	{
		using Container = UnorderedMap<K, V, Alloc>;

		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& stream, const Container& container) noexcept
		{
			MH_SER_ASSERT(stream.serialize(container.size()));

			for (auto& [k, v] : container)
			{
				MH_SER_ASSERT(stream.serialize(k));
				MH_SER_ASSERT(stream.serialize(v));
			}

			return true;
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& stream, Container& container) noexcept
		{
			auto size = container.size();
			MH_SER_ASSERT(stream.serialize(size));

			if constexpr (Stream::reading)
				container.reserve(size);

			for (size_t i = 0; i < size; ++i)
			{
				K k;
				V v;

				MH_SER_ASSERT(stream.serialize(k));
				MH_SER_ASSERT(stream.serialize(v));

				container.emplace(std::move(k), std::move(v));
			}

			return true;
		}
	};

	template<typename K, typename V>
	struct SerializeTraits<std::pair<K, V>>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, std::pair<K, V>> pair) noexcept
		{
			MH_SER_ASSERT(stream.serialize(pair.first));
			MH_SER_ASSERT(stream.serialize(pair.second));

			return true;
		}
	};
}