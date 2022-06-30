#pragma once

#include "Mahakam/Core/Core.h"

#include "AssetDatabase.h"

namespace Mahakam
{
	template<typename T>
	class Asset
	{
	private:
		template<typename T2>
		friend class Asset;

		uint64_t m_ID = 0;

		Ref<T> m_Ptr = nullptr;

	public:
		Asset() : m_ID(0), m_Ptr(nullptr) {}

		Asset(const nullptr_t&) {}

		Asset(Ref<T> ptr)
		{
			m_Ptr = ptr;
		}

		Asset(uint64_t id) : m_ID(id)
		{
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
		}

		Asset(const std::filesystem::path& importPath)
		{
			m_ID = AssetDatabase::ReadAssetInfo(importPath).ID;
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
		}

		Asset(Asset&& other) noexcept
			: m_ID(other.m_ID), m_Ptr(other.m_Ptr) {}

		Asset(const Asset& other) noexcept
			: m_ID(other.m_ID), m_Ptr(other.m_Ptr)
		{
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
		}

		~Asset()
		{
			if (m_ID)
				AssetDatabase::DeregisterAsset(m_ID);
		}

		template<typename T2>
		operator Asset<T2>() const
		{
			auto ptr = std::static_pointer_cast<T2>(this->m_Ptr);

			if (m_ID != 0)
				return Asset<T2>(m_ID);
			else
				return Asset<T2>(ptr);
		}

		template<typename T2>
		Asset& operator=(Asset<T2>&& rhs)
		{
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
			return *this;
		}

		Asset& operator=(Asset&& rhs) noexcept
		{
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
			return *this;
		}

		template<typename T2>
		Asset& operator=(const Asset<T2>& rhs)
		{
			if (m_ID)
				AssetDatabase::DeregisterAsset(m_ID);
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
			return *this;
		}

		Asset& operator=(const Asset& rhs)
		{
			if (m_ID)
				AssetDatabase::DeregisterAsset(m_ID);
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			if (m_ID)
				AssetDatabase::RegisterAsset(m_ID);
			return *this;
		}

		Asset& operator=(const nullptr_t& rhs)
		{
			if (m_ID)
				AssetDatabase::DeregisterAsset(m_ID);
			m_Ptr = nullptr;
			m_ID = 0;
			return *this;
		}

		void Save(const std::filesystem::path& filepath, const std::filesystem::path& importPath)
		{
			m_ID = AssetDatabase::SaveAsset(Get(), filepath, importPath);
			if (m_Ptr)
			{
				AssetDatabase::RegisterAsset(m_ID);
				m_Ptr = nullptr;
			}
		}

		uint64_t GetID() const { return m_ID; }

		std::filesystem::path GetImportPath() const { return AssetDatabase::GetAssetImportPath(m_ID); }

		Ref<T> Get()
		{
			if (m_ID != 0)
				return AssetDatabase::LoadAsset<T>(m_ID);
			return m_Ptr;
		}

		Ref<T> Get() const
		{
			if (m_ID != 0)
				return AssetDatabase::LoadAsset<T>(m_ID);
			return m_Ptr;
		}

		T* Ptr()
		{
			return Get().get();
		}

		T* Ptr() const
		{
			return Get().get();
		}

		Ref<T> operator->() noexcept { return Get(); }

		Ref<T> operator->() const noexcept { return Get(); }

		explicit operator bool() const noexcept { return Get() != nullptr; }
	};

	template<typename T>
	bool operator==(const Asset<T>& _Left, nullptr_t) noexcept
	{
		return _Left.Ptr() == nullptr;
	}

	template<typename T1, typename T2>
	bool operator==(const Asset<T1>& _Left, const Asset<T2>& _Right) noexcept
	{
		return _Left.Ptr() == _Right.Ptr();
	}
}

namespace std
{
	template<typename _Ty>
	struct hash<Mahakam::Asset<_Ty>>
	{
		size_t operator()(const Mahakam::Asset<_Ty>& _Keyval) const noexcept
		{
			return hash<_Ty*>()(_Keyval.Ptr());
		}
	};
}