#pragma once

#include "SharedLibrary.h"

#include <ktl/allocators/cascading.h>
#include <ktl/allocators/fallback.h>
#include <ktl/allocators/freelist.h>
#include <ktl/allocators/linear_allocator.h>
#include <ktl/allocators/mallocator.h>
#include <ktl/allocators/reference.h>
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
        
		using AllocatorType = ktl::segragator<MAX_LINEAR_SIZE, Fallback, ktl::mallocator>;

    public:
		struct Deleter
		{
			void (*Deleter)(void*);

			void operator()(void* ptr)
			{
				Deleter(ptr);
			}
		};

		// No need for shared<T>, we can use reference<T> instead since it's global
		using ReferenceAllocator = ktl::reference<AllocatorType>;

		template<typename T>
		using BaseAllocator = ktl::type_allocator<T, ktl::reference<AllocatorType>>;

		template<typename T>
		using BaseTraits = std::allocator_traits<BaseAllocator<T>>;

		template<typename T>
		static BaseAllocator<T> GetAllocator()
		{
			return BaseAllocator<T>(GetAllocatorImpl());
		}

		template<typename T>
		static T* Allocate(size_t n)
		{
			return GetAllocator<T>().allocate(n);
		}

		template<typename T>
		static void Deallocate(T* p, size_t n)
		{
			GetAllocator<T>().deallocate(p, n);
		}

		template<typename T, typename ...Args>
		static void Construct(T* p, Args&&... args)
		{
			BaseAllocator<T> alloc(GetAllocator<T>());
			BaseTraits<T>::construct(alloc, p, std::forward<Args>(args)...);
		}

		template<typename T>
		static void Deconstruct(T* p)
		{
			BaseAllocator<T> alloc(GetAllocator<T>());
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
		inline static AllocatorType s_Alloc{ s_Buffer };

		MH_DECLARE_FUNC(GetAllocatorImpl, ReferenceAllocator);
	};
}