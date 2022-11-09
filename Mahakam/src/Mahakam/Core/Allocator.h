#pragma once

#include <ktl/allocators/cascading_allocator.h>
#include <ktl/allocators/freelist_allocator.h>
#include <ktl/allocators/linear_allocator.h>
#include <ktl/allocators/mallocator.h>
#include <ktl/allocators/segragator_allocator.h>

#include <cstdint>

namespace Mahakam
{
	class Allocator
	{
	public:
		template<typename T>
		using BaseAllocator = ktl::type_segragator_allocator<T, 512,
			ktl::cascading_allocator<ktl::freelist_allocator<0, 512, ktl::linear_allocator<512 * 64>>>,
			ktl::mallocator>;

		template<typename T>
		using BaseTraits = std::allocator_traits<BaseAllocator<T>>;

		template<typename T>
		static BaseAllocator<T> GetAllocator()
		{
			return s_Alloc;
		}

		template<typename T>
		static T* Allocate(size_t n)
		{
			return reinterpret_cast<T*>(s_Alloc.allocate(n * sizeof(T)));
		}

		template<typename T>
		static void Deallocate(T* p, size_t n)
		{
			s_Alloc.deallocate(reinterpret_cast<uint8_t*>(p), n * sizeof(T));
		}

		template<typename T, typename ...Args>
		static void Construct(T* p, Args&&... args)
		{
			BaseAllocator<T> alloc = static_cast<BaseAllocator<T>>(s_Alloc);
			BaseTraits<T>::construct(alloc, p, std::forward<Args>(args)...);
		}

		template<typename T>
		static void Deconstruct(T* p)
		{
			BaseAllocator<T> alloc = static_cast<BaseAllocator<T>>(s_Alloc);
			BaseTraits<T>::destroy(alloc, p);
		}

	private:
		inline static BaseAllocator<uint8_t> s_Alloc;
	};
}