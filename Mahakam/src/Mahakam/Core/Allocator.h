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
		static BaseAllocator<T> GetAllocator()
		{
			return m_Alloc;
		}

		template<typename T>
		static T* Allocate(size_t n)
		{
			return reinterpret_cast<T*>(m_Alloc.allocate(n * sizeof(T)));
		}

		template<typename T>
		static void Deallocate(T* p, size_t n)
		{
			m_Alloc.deallocate(reinterpret_cast<uint8_t*>(p), n * sizeof(T));
		}

		template<typename T, typename ...Args>
		static void Construct(T* p, Args&&... args)
		{
			std::allocator_traits<BaseAllocator<T>>::construct(static_cast<BaseAllocator<T>>(m_Alloc), p, std::forward<Args>(args)...);
		}

		template<typename T>
		static void Deconstruct(T* p)
		{
			std::allocator_traits<BaseAllocator<T>>::destroy(static_cast<BaseAllocator<T>>(m_Alloc), p);
		}

	private:
		inline static BaseAllocator<uint8_t> m_Alloc;
	};
}