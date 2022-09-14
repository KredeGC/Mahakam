#pragma once

#include <cstring>
#include <iterator>
#include <memory>
#include <utility>

namespace Mahakam
{
	template<typename T, typename Alloc = std::allocator<T>>
	class darray : private Alloc
	{
		static_assert(std::is_standard_layout<T>(), "Template class needs standard layout");
	public:
		typedef T* iterator;
		typedef const T* const_iterator;

	private:
		T* m_Begin;
		T* m_End;

	public:
		darray(const Alloc& allocator = Alloc()) noexcept :
			Alloc(allocator),
			m_Begin(nullptr),
			m_End(nullptr) {}

		darray(size_t n, const Alloc& allocator = Alloc()) :
			Alloc(allocator),
			m_Begin(Alloc::allocate(n)),
			m_End(m_Begin + n) {}

		darray(size_t n, const T& value, const Alloc& allocator = Alloc()) :
			Alloc(allocator),
			m_Begin(Alloc::allocate(n)),
			m_End(m_Begin + n)
		{
			std::uninitialized_fill_n<T*, size_t>(m_Begin, n, value);
		}

		darray(std::initializer_list<T> initializer, const Alloc& allocator = Alloc()) :
			Alloc(allocator),
			m_Begin(Alloc::allocate(initializer.size())),
			m_End(m_Begin + initializer.size())
		{
			T* dst = m_Begin;
			// Using a normal auto for loop gives illegal indirection error
			for (auto i = initializer.begin(); i != initializer.end(); i++)
			{
				*dst = *i;
				dst++;
			}
		}

		darray(const T* first, const T* last, const Alloc& allocator = Alloc()) :
			Alloc(allocator),
			m_Begin(Alloc::allocate(size_t(last - first))),
			m_End(m_Begin + size_t(last - first))
		{
			const size_t n = last - first;
			memcpy(m_Begin, first, n * sizeof(T));
		}

		darray(const darray<T, Alloc>& other) :
			Alloc(std::allocator_traits<Alloc>::select_on_container_copy_construction(static_cast<Alloc>(other))),
			m_Begin(Alloc::allocate(other.size())),
			m_End(m_Begin + other.size())
		{
			memcpy(m_Begin, other.m_Begin, other.size() * sizeof(T));
		}

		darray(darray<T, Alloc>&& other) :
			Alloc(std::move(other)),
			m_Begin(other.m_Begin),
			m_End(other.m_End)
		{
			other.m_Begin = nullptr; // Is this a memory leak?
			other.m_End = nullptr;
		}

		~darray() noexcept
		{
			if (m_Begin != nullptr)
				Alloc::deallocate(m_Begin, size());
		}

		darray& operator=(const darray<T, Alloc>& other)
		{
			const size_t n = other.size();

			if (n != size())
			{
				T* alBlock = Alloc::allocate(n);

				if (m_Begin != nullptr)
					Alloc::deallocate(m_Begin, size());

				m_Begin = alBlock;
				m_End = m_Begin + n;
			}

			memcpy(m_Begin, other.m_Begin, n * sizeof(T));
			return *this;
		}

		darray& operator=(darray<T, Alloc>&& other) noexcept
		{
			if (m_Begin != nullptr)
				Alloc::deallocate(m_Begin, size());

			m_Begin = other.m_Begin;
			m_End = other.m_End;

			other.m_Begin = nullptr;
			other.m_End = nullptr;
			return *this;
		}

		iterator begin() noexcept { return m_Begin; }

		const_iterator begin() const noexcept { return m_Begin; }

		iterator end() noexcept { return m_End; }

		const_iterator end() const noexcept { return m_End; }

		std::reverse_iterator<T*> rbegin() noexcept { return std::reverse_iterator(m_End); }

		std::reverse_iterator<const T*> rbegin() const noexcept { return std::reverse_iterator(m_End); }

		std::reverse_iterator<T*> rend() noexcept { return std::reverse_iterator(m_Begin); }

		std::reverse_iterator<const T*> rend() const noexcept { return std::reverse_iterator(m_Begin); }


		size_t size() const noexcept { return m_End - m_Begin; }

		bool empty() const noexcept { return m_Begin == m_End; }


		T* data() noexcept { return m_Begin; }

		const T* data() const noexcept { return m_Begin; }

		T& at(size_t index) const noexcept { return m_Begin[index]; }


		T& operator[](size_t index) noexcept { return m_Begin[index]; }

		const T& operator[](size_t index) const noexcept { return m_Begin[index]; }


		void resize(size_t n)
		{
			if (size() != n)
			{
				size_t curSize = size();
				T* alBlock = Alloc::allocate(n);

				if (m_Begin != nullptr)
				{
					memcpy(alBlock, m_Begin, std::min(curSize, n) * sizeof(T));
					Alloc::deallocate(m_Begin, curSize);
				}

				m_Begin = alBlock;
				m_End = m_Begin + n;
			}
		}

		void assign(const T* first, const T* last)
		{
			const size_t n = last - first;

			if (n != size())
			{
				T* alBlock = Alloc::allocate(n);

				if (m_Begin != nullptr)
					Alloc::deallocate(m_Begin, size());

				m_Begin = alBlock;
				m_End = m_Begin + n;
			}

			memcpy(m_Begin, first, n * sizeof(T));
		}

		void clear() { m_End = m_Begin; }
	};
}