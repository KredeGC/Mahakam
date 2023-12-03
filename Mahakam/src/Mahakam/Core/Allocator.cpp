#include "Mahakam/mhpch.h"

#include "Allocator.h"

namespace Mahakam
{
	//Allocator::ReferenceAllocator Allocator::GetAllocatorImpl()
	MH_DEFINE_FUNC(Allocator::GetAllocatorImpl, Allocator::ReferenceAllocator)
	{
		return ReferenceAllocator(s_Alloc);
	};
}