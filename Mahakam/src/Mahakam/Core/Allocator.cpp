#include "Mahakam/mhpch.h"

#include "Allocator.h"

namespace Mahakam
{
	//Allocator::BaseAllocator<uint8_t> Allocator::GetAllocatorImpl()
	MH_DEFINE_FUNC(Allocator::GetAllocatorImpl, Allocator::BaseAllocator<uint8_t>)
	{
		return s_Alloc;
	};
}