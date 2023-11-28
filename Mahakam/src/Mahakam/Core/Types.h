#pragma once

#include "Core.h"
#include "Allocator.h"

#ifdef MH_ENABLE_ASSERT
#define KTL_DEBUG_ASSERT
#endif
#include <ktl/containers/trivial_array.h>
#include <ktl/containers/trivial_vector.h>

#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace Mahakam
{
	template<typename T, typename Alloc = std::allocator<T>>
	using TrivialArray = ktl::trivial_array<T, Alloc>;

	template<typename T, typename Alloc = std::allocator<T>>
	using TrivialVector = ktl::trivial_vector<T, Alloc>;

	template<typename K, typename V, typename Alloc = std::allocator<std::pair<const K, V>>>
	using UnorderedMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, Alloc>;

	template<typename K>
	using UnorderedSet = std::unordered_set<K>;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::allocate_shared<T>(Allocator::GetAllocator<T>(), std::forward<Args>(args)...);
	}

	template<typename T>
	constexpr WeakRef<T> CreateWeakRef(const Ref<T>& ref)
	{
		return WeakRef<T>(ref);
	}

	template<typename T, typename T2>
	constexpr Scope<T> StaticCastScope(Scope<T2> ptr)
	{
		return std::static_pointer_cast<T>(ptr);
	}

	template<typename T, typename T2>
	constexpr Ref<T> DynamicCastRef(Ref<T2> ptr)
	{
		return std::dynamic_pointer_cast<T>(ptr);
	}

	template<typename T, typename T2>
	constexpr Ref<T> StaticCastRef(Ref<T2> ptr)
	{
		return std::static_pointer_cast<T>(ptr);
	}

	template<typename T>
	constexpr T Bit(T n)
	{
		return static_cast<T>(1ULL << n);
	}

	inline constexpr size_t Bit(size_t n)
	{
		return Bit<size_t>(n);
	}
}