#include "Mahakam/mhpch.h"

#include "Core.h"

// This only works for non-inlined implementations of the shared_ptr
// Which is unfortunately not most implementations...
namespace Mahakam
{
	class StorageBuffer;
	class UniformBuffer;
}

template class std::shared_ptr<Mahakam::StorageBuffer>;
template class std::shared_ptr<Mahakam::UniformBuffer>;