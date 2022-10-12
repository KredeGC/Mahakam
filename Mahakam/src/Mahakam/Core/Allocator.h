#pragma once

#include <ktl/allocators/composite_allocator.h>
#include <ktl/allocators/freelist_allocator.h>
#include <ktl/allocators/mallocator.h>
#include <ktl/allocators/pre_allocator.h>

#include <cstdint>

namespace Mahakam
{
	class Allocator
	{
	private:
		template<typename T>
		using BaseAllocator = ktl::type_composite_allocator<T, ktl::freelist_allocator<0, 256, 8, 64, ktl::pre_allocator<16384>>, ktl::mallocator>;

		inline static BaseAllocator<uint8_t> m_Alloc;

	public:
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
			std::allocator_traits<T>::construct(m_Alloc, p, std::forward<Args>(args)...);
		}

		template<typename T>
		static void Deconstruct(T* p)
		{
			std::allocator_traits<T>::destroy(m_Alloc, p);
		}
	};
}