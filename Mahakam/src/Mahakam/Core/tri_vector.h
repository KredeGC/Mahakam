#pragma once

#include <cstring>
#include <iterator>
#include <memory>
#include <utility>

namespace Mahakam
{
	template<typename T, typename Alloc = std::allocator<T>>
	class tri_vector : private Alloc
	{
		static_assert(std::is_standard_layout<T>(), "Template class needs standard layout");
	private:
		T* m_Begin;
		T* m_End;
		T* m_EndMax;

	public:
		tri_vector(const Alloc& allocator = Alloc()) noexcept :
			Alloc(allocator),
			m_Begin(nullptr),
			m_End(nullptr),
			m_EndMax(nullptr) {}

		tri_vector(size_t n, const Alloc& allocator = Alloc()) :
			Alloc(allocator),
			m_Begin(Alloc::allocate(n)),
			m_End(m_Begin + n),
			m_EndMax(m_End) {}

		tri_vector(std::initializer_list<T> initializer, const Alloc& allocator = Alloc()) :
			Alloc(allocator),
			m_Begin(Alloc::allocate(initializer.size())),
			m_End(m_Begin + initializer.size()),
			m_EndMax(m_End)
		{
			T* dst = m_Begin;
			// Using a normal auto for loop gives illegal indirection error
			for (auto i = initializer.begin(); i != initializer.end(); i++)
			{
				*dst = *i;
				dst++;
			}
		}

		tri_vector(const tri_vector<T, Alloc>& other) :
			Alloc(std::allocator_traits<Alloc>::select_on_container_copy_construction(static_cast<Alloc>(other))),
			m_Begin(Alloc::allocate(other.size())),
			m_End(m_Begin + other.size()),
			m_EndMax(m_End) {}


		tri_vector(tri_vector<T, Alloc>&& other) :
			Alloc(std::move(other)),
			m_Begin(other.m_Begin),
			m_End(other.m_End),
			m_EndMax(other.m_EndMax)
		{
			other.m_Begin = nullptr;
			other.m_End = nullptr;
			other.m_EndMax = nullptr;
		}

		~tri_vector() noexcept
		{
			if (m_Begin != nullptr)
				Alloc::deallocate(m_Begin, capacity());
		}

		tri_vector& operator=(const tri_vector<T, Alloc>& other)
		{
			const size_t n = other.size();

			if (n > capacity()) {
				T* alBlock = Alloc::allocate(n);

				if (m_Begin != nullptr)
					Alloc::deallocate(m_Begin, capacity());

				m_Begin = alBlock;
				m_End = m_Begin + n;
				m_EndMax = m_End;
			}

			memcpy(m_Begin, other.m_Begin, n * sizeof(T));
			return *this;
		}

		tri_vector& operator=(tri_vector<T, Alloc>&& other) noexcept
		{
			if (m_Begin != nullptr)
				Alloc::deallocate(m_Begin, capacity());

			m_Begin = other.m_Begin;
			m_End = other.m_End;
			m_EndMax = other.m_EndMax;

			other.m_Begin = nullptr;
			other.m_End = nullptr;
			other.m_EndMax = nullptr;
			return *this;
		}

		T* begin() noexcept { return m_Begin; }

		const T* begin() const noexcept { return m_Begin; }

		T* end() noexcept { return m_End; }

		const T* end() const noexcept { return m_End; }

		std::reverse_iterator<T*> rbegin() noexcept { return std::reverse_iterator(m_End); }

		std::reverse_iterator<const T*> rbegin() const noexcept { return std::reverse_iterator(m_End); }

		std::reverse_iterator<T*> rend() noexcept { return std::reverse_iterator(m_Begin); }

		std::reverse_iterator<const T*> rend() const noexcept { return std::reverse_iterator(m_Begin); }

		size_t size() const noexcept { return m_End - m_Begin; }

		size_t capacity() const noexcept { return m_EndMax - m_Begin; }

		bool empty() const noexcept { return m_Begin == m_End; }

		T* data() noexcept { return m_Begin; }

		const T* data() const noexcept { return m_Begin; }

		T& operator[](size_t index) noexcept { return m_Begin[index]; }

		const T& operator[](size_t index) const noexcept { return m_Begin[index]; }

		void resize(size_t n)
		{
			if (capacity() < n)
			{
				size_t alSize = size() + std::max(size(), n);
				T* alBlock = Alloc::allocate(alSize);
				memcpy(alBlock, m_Begin, size() * sizeof(T));

				if (m_Begin != nullptr)
					Alloc::deallocate(m_Begin, capacity());

				m_Begin = alBlock;
				m_EndMax = m_Begin + alSize;
			}
			m_End = m_Begin + n;
		}

		void reserve(size_t n)
		{
			if (capacity() < n)
			{
				size_t curSize = size();
				T* alBlock = Alloc::allocate(n);
				memcpy(alBlock, m_Begin, curSize * sizeof(T));

				if (m_Begin != nullptr)
					Alloc::deallocate(m_Begin, capacity());

				m_Begin = alBlock;
				m_End = m_Begin + curSize;
				m_EndMax = m_Begin + n;
			}
		}

		void push_back(const T& element)
		{
			if (m_End == m_EndMax)
				expand();
			*m_End = element;
			m_End++;
		}

		void push_back(T&& element)
		{
			if (m_End == m_EndMax)
				expand();
			*m_End = std::move(element);
			m_End++;
		}

		template<typename... Args>
		void emplace_back(Args&&... args)
		{
			if (m_End == m_EndMax)
				expand();
			*m_End = T(std::forward<Args>(args)...);
			m_End++;
		}

		void pop_back() { m_End--; }

		void clear() { m_End = m_Begin; }

	private:
		void expand()
		{
			size_t curSize = size();
			size_t alSize = curSize + std::max(curSize, size_t(1UL));

			T* alBlock = Alloc::allocate(alSize);
			memcpy(alBlock, m_Begin, curSize * sizeof(T));

			if (m_Begin != nullptr)
				Alloc::deallocate(m_Begin, capacity());

			m_Begin = alBlock;
			m_End = m_Begin + curSize;
			m_EndMax = m_Begin + alSize;
		}
	};
}