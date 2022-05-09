#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Core/AssetDatabase.h"

namespace Mahakam
{
	template<typename T>
	class Asset
	{
	private:
		template <class T2>
		friend class Asset;

		uint64_t m_ID = 0;

		Ref<T> m_Ptr = nullptr;

	public:
		Asset() : m_ID(0), m_Ptr(nullptr) {}

		Asset(Ref<T> ptr)
		{
			m_Ptr = ptr;
		}

		Asset(uint64_t id) : m_ID(id)
		{
			m_Ptr = AssetDatabase::LoadAsset<T>(m_ID);
		}

		Asset(const std::filesystem::path& filepath)
		{
			m_ID = AssetDatabase::GetAssetInfo(filepath).ID;
			m_Ptr = AssetDatabase::LoadAsset<T>(m_ID);
		}

		void SetID(uint64_t id)
		{
			m_ID = id;
		}

		template<class T2>
		operator Asset<T2>() const
		{
			auto ptr = std::static_pointer_cast<T2>(this->m_Ptr);

			if (m_ID != 0)
				return Asset<T2>(m_ID);
			else
				return Asset<T2>(ptr);
		}

		template<class T2>
		Asset& operator=(const Asset<T2>& rhs)
		{
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			return *this;
		}

		Asset& operator=(const Asset& rhs)
		{
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			return *this;
		}

		Asset& operator=(const nullptr_t& rhs)
		{
			m_Ptr = nullptr;
			m_ID = 0;
			return *this;
		}

		uint64_t GetID() const
		{
			return m_ID;
		}

		Ref<T> Get()
		{
			if (m_ID != 0)
				m_Ptr = AssetDatabase::LoadAsset<T>(m_ID);
			return m_Ptr;
		}

		Ref<T> Get() const
		{
			if (m_ID != 0)
				return AssetDatabase::LoadAsset<T>(m_ID);
			return m_Ptr;
		}

		Ref<T> operator->() noexcept
		{
			return Get();
		}

		Ref<T> operator->() const noexcept
		{
			return Get();
		}

		operator uint64_t() const noexcept
		{
			return m_ID;
		}

		explicit operator bool() const noexcept
		{
			return Get() != nullptr;
		}

		template<typename ... Args>
		inline static Asset<T> Create(Args&& ... args)
		{
			return Asset<T>(CreateRef<T>(std::forward<Args>(args)...));
		}
	};

	template<class T>
	bool operator==(const Asset<T>& _Left, nullptr_t) noexcept {
		return _Left.Get().get() == nullptr;
	}

	template <class T1, class T2>
	bool operator==(const Asset<T1>& _Left, const Asset<T2>& _Right) noexcept {
		return _Left.Get().get() == _Right.Get().get();
	}
}

namespace std
{
	template <class _Ty>
	struct hash<Mahakam::Asset<_Ty>> {
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef Mahakam::Asset<_Ty> _ARGUMENT_TYPE_NAME;
		_CXX17_DEPRECATE_ADAPTOR_TYPEDEFS typedef size_t _RESULT_TYPE_NAME;

		_NODISCARD size_t operator()(const Mahakam::Asset<_Ty>& _Keyval) const noexcept {
			return hash<typename _Ty*>()(_Keyval.Get().get());
		}
	};
}