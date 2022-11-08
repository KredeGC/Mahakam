#pragma once

namespace ktl
{
	class null_allocator
	{
	public:
		null_allocator() noexcept = default;

		null_allocator(const null_allocator& other) noexcept = default;

		null_allocator(null_allocator&& other) noexcept = default;

		void* allocate(size_t n)
		{
			return nullptr;
		}

		void deallocate(void* p, size_t n) noexcept
		{
			
		}

		bool owns(void* p)
		{
			return p == nullptr;
		}

		bool operator==(const null_allocator& rhs) noexcept
		{
			return true;
		}

		bool operator!=(const null_allocator& rhs) noexcept
		{
			return false;
		}
	};
}