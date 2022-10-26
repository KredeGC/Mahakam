#pragma once

#include "Mahakam/Core/Core.h"

#include "AssetDatabase.h"

#include <cstddef>

namespace Mahakam
{
	template<typename T>
	class Asset
	{
	private:
		template<typename T2>
		friend class Asset;

		// 0 is guaranteed to be invalid
		uint64_t m_ID = 0;

		Ref<T> m_Ptr = nullptr;

	public:
		Asset();

		Asset(const std::nullptr_t&) {}
		
		explicit Asset(Ref<T> ptr);
		
		explicit Asset(uint64_t id);

		Asset(const std::filesystem::path& importPath);

		Asset(Asset&& other) noexcept;

		Asset(const Asset& other) noexcept;

		~Asset();

		template<typename T2>
		operator Asset<T2>() const
		{
			auto ptr = StaticCastRef<T2>(this->m_Ptr);

			if (m_ID != 0)
				return Asset<T2>(m_ID);
			else
				return Asset<T2>(ptr);
		}

		template<typename T2>
		Asset& operator=(Asset<T2>&& rhs)
		{
			// Remember to clear if we already have something
			if (m_ID)
				AssetDatabase::DeregisterAsset(m_ID);
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			// Invalidate the rhs, as it's destructor is still called
			rhs.m_Ptr = nullptr;
			rhs.m_ID = 0;
			return *this;
		}

		Asset<T>& operator=(Asset<T>&& rhs) noexcept;

		template<typename T2>
		Asset& operator=(const Asset<T2>& rhs)
		{
			// Remember to clear if we already have something
			if (m_ID)
				AssetDatabase::DeregisterAsset(m_ID);
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			// Register our new ID
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
			return *this;
		}

		Asset<T>& operator=(const Asset<T>& rhs);

		Asset& operator=(const std::nullptr_t& rhs);

		void Save(const std::filesystem::path& filepath, const std::filesystem::path& importPath);

		uint64_t GetID() const;

		std::filesystem::path GetImportPath() const;

		Ref<T> RefPtr();

		Ref<T> RefPtr() const;

		T* get();

		T* get() const;

		T* operator->() noexcept;

		T* operator->() const noexcept;

		explicit operator bool() const noexcept;
	};


	template<typename T>
	bool operator==(const Asset<T>& _Left, std::nullptr_t) noexcept
	{
		return _Left.get() == nullptr;
	}

	template<typename T1, typename T2>
	bool operator==(const Asset<T1>& _Left, const Asset<T2>& _Right) noexcept
	{
		return _Left.get() == _Right.get();
	}

	template<typename T>
	Asset<T>::Asset() : m_ID(0), m_Ptr(nullptr) {}

	template<typename T>
	Asset<T>::Asset(Ref<T> ptr)
	{
		m_Ptr = ptr;
	}

	template<typename T>
	Asset<T>::Asset(uint64_t id) : m_ID(id)
	{
		// Register if the ID is valid
		if (m_ID)
			AssetDatabase::RegisterAsset(m_ID);
	}

	template<typename T>
	Asset<T>::Asset(const std::filesystem::path& importPath)
	{
		// Register if the ID is valid
		m_ID = AssetDatabase::ReadAssetInfo(importPath).ID;
		if (m_ID)
			AssetDatabase::RegisterAsset(m_ID);
	}

	template<typename T>
	Asset<T>::Asset(Asset&& other) noexcept
		: m_ID(other.m_ID), m_Ptr(other.m_Ptr)
	{
		// Invalidate the other, as it's destructor is still called
		other.m_ID = 0;
		other.m_Ptr = nullptr;
	}

	template<typename T>
	Asset<T>::Asset(const Asset& other) noexcept
		: m_ID(other.m_ID), m_Ptr(other.m_Ptr)
	{
		// Register if the ID is valid
		if (m_ID)
			AssetDatabase::RegisterAsset(m_ID);
	}

	template<typename T>
	Asset<T>::~Asset()
	{
		// Remember to clear on delete
		if (m_ID)
			AssetDatabase::DeregisterAsset(m_ID);
	}

	template<typename T>
	Asset<T>& Asset<T>::operator=(Asset<T>&& rhs) noexcept
	{
		// Remember to clear if we already have something
		if (m_ID)
			AssetDatabase::DeregisterAsset(m_ID);
		m_Ptr = rhs.m_Ptr;
		m_ID = rhs.m_ID;
		// Invalidate the rhs, as it's destructor is still called
		rhs.m_Ptr = nullptr;
		rhs.m_ID = 0;
		return *this;
	}

	template<typename T>
	Asset<T>& Asset<T>::operator=(const Asset<T>& rhs)
	{
		// Remember to clear if we already have something
		if (m_ID)
			AssetDatabase::DeregisterAsset(m_ID);
		m_Ptr = rhs.m_Ptr;
		m_ID = rhs.m_ID;
		// Register our new ID
		if (m_ID)
			AssetDatabase::RegisterAsset(m_ID);
		return *this;
	}

	template<typename T>
	Asset<T>& Asset<T>::operator=(const std::nullptr_t& rhs)
	{
		// Remember to clear if we already have something
		if (m_ID)
			AssetDatabase::DeregisterAsset(m_ID);
		m_Ptr = nullptr;
		m_ID = 0;
		return *this;
	}

	template<typename T>
	void Asset<T>::Save(const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		m_ID = AssetDatabase::SaveAsset(RefPtr(), filepath, importPath);
		if (m_Ptr)
		{
			AssetDatabase::RegisterAsset(m_ID);
			m_Ptr = nullptr;
		}
	}

	template<typename T>
	uint64_t Asset<T>::GetID() const
	{
		return m_ID;
	}

	template<typename T>
	std::filesystem::path Asset<T>::GetImportPath() const
	{
		return AssetDatabase::GetAssetImportPath(m_ID);
	}

	template<typename T>
	Ref<T> Asset<T>::RefPtr()
	{
		if (m_ID != 0)
			return AssetDatabase::LoadAsset<T>(m_ID);
		return m_Ptr;
	}

	template<typename T>
	Ref<T> Asset<T>::RefPtr() const
	{
		if (m_ID != 0)
			return AssetDatabase::LoadAsset<T>(m_ID);
		return m_Ptr;
	}

	template<typename T>
	T* Asset<T>::get()
	{
		return RefPtr().get();
	}

	template<typename T>
	T* Asset<T>::get() const
	{
		return RefPtr().get();
	}

	template<typename T>
	T* Asset<T>::operator->() noexcept
	{
		return get();
	}

	template<typename T>
	T* Asset<T>::operator->() const noexcept
	{
		return get();
	}

	template<typename T>
	Asset<T>::operator bool() const noexcept
	{
		return RefPtr() != nullptr;
	}
}

namespace std
{
	template<typename _Ty>
	struct hash<Mahakam::Asset<_Ty>>
	{
		size_t operator()(const Mahakam::Asset<_Ty>& _Keyval) const noexcept
		{
			return hash<_Ty*>()(_Keyval.get());
		}
	};
}