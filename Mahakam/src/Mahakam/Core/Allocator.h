#pragma once

#include "SharedLibrary.h"

#include <ktl/allocators/cascading.h>
#include <ktl/allocators/fallback.h>
#include <ktl/allocators/freelist.h>
#include <ktl/allocators/linear_allocator.h>
#include <ktl/allocators/mallocator.h>
#include <ktl/allocators/segragator.h>
#include <ktl/allocators/shared.h>
#include <ktl/allocators/stack_allocator.h>

#include <cstdint>

namespace Mahakam
{
	class Allocator
	{
	private:
        static constexpr size_t MAX_STACK_SIZE = 128;
        static constexpr size_t MAX_LINEAR_SIZE = 512;
        static constexpr size_t BUFFER_SIZE = 64;
        
        using Stack = ktl::stack_allocator<MAX_STACK_SIZE * BUFFER_SIZE>;
        using Linear = ktl::linear_allocator<MAX_LINEAR_SIZE * BUFFER_SIZE>;
        
        using FreelistStack = ktl::freelist<0, MAX_STACK_SIZE, Stack>;
        using FreelistLinear = ktl::freelist<0, MAX_LINEAR_SIZE, Linear>;
        
        using CascadingFreelistLinear = ktl::cascading<FreelistLinear>;
        
        using Fallback = ktl::fallback<FreelistStack, CascadingFreelistLinear>;
        
    public:
		template<typename T>
		using BaseAllocator = ktl::type_shared_segragator_allocator<T, MAX_LINEAR_SIZE, Fallback, ktl::mallocator>;

		template<typename T>
		using BaseTraits = std::allocator_traits<BaseAllocator<T>>;

		template<typename T>
		static BaseAllocator<T> GetAllocator()
		{
			return GetAllocatorImpl();
		}

		template<typename T>
		static T* Allocate(size_t n)
		{
			return static_cast<BaseAllocator<T>>(GetAllocatorImpl()).allocate(n);
		}

		template<typename T>
		static void Deallocate(T* p, size_t n)
		{
			static_cast<BaseAllocator<T>>(GetAllocatorImpl()).deallocate(p, n);
		}

		template<typename T, typename ...Args>
		static void Construct(T* p, Args&&... args)
		{
			BaseAllocator<T> alloc(static_cast<BaseAllocator<T>>(GetAllocatorImpl()));
			BaseTraits<T>::construct(alloc, p, std::forward<Args>(args)...);
		}

		template<typename T>
		static void Deconstruct(T* p)
		{
			BaseAllocator<T> alloc(static_cast<BaseAllocator<T>>(GetAllocatorImpl()));
			BaseTraits<T>::destroy(alloc, p);
		}

		template<typename T, typename ...Args>
		static T* New(Args&&... args)
		{
			T* p = Allocate<T>(1);
			Construct(p, std::forward<Args>(args)...);
			return p;
		}

		template<typename T>
		static void Delete(T* p)
		{
			Deconstruct(p);
			Deallocate(p, 1);
		}

	private:
        inline static ktl::stack<MAX_STACK_SIZE * BUFFER_SIZE> s_Buffer;
		inline static BaseAllocator<uint8_t> s_Alloc{ s_Buffer };

		MH_DECLARE_FUNC(GetAllocatorImpl, BaseAllocator<uint8_t>);
	};
}