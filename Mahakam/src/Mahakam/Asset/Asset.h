#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Core/AssetDatabase.h"

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
		Asset& operator=(const Asset<T2>& rhs)
		{
			AssetDatabase::DeregisterAsset(m_ID);
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			AssetDatabase::RegisterAsset(m_ID);
			return *this;
		}

		Asset& operator=(const Asset& rhs)
		{
			AssetDatabase::DeregisterAsset(m_ID);
			m_Ptr = rhs.m_Ptr;
			m_ID = rhs.m_ID;
			AssetDatabase::RegisterAsset(m_ID);
			return *this;
		}

		Asset& operator=(const nullptr_t& rhs)
		{
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

		Ref<T> operator->() noexcept { return Get(); }

		Ref<T> operator->() const noexcept { return Get(); }

		explicit operator bool() const noexcept { return Get() != nullptr; }

		// TODO: Remove this and just do it explicitly in each asset type constructor
		template<typename ... Args>
		inline static Asset<T> Create(Args&& ... args)
		{
			return Asset<T>(CreateRef<T>(std::forward<Args>(args)...));
		}
	};

	template<typename T>
	bool operator==(const Asset<T>& _Left, nullptr_t) noexcept
	{
		return _Left.Get().get() == nullptr;
	}

	template<typename T1, typename T2>
	bool operator==(const Asset<T1>& _Left, const Asset<T2>& _Right) noexcept
	{
		return _Left.Get().get() == _Right.Get().get();
	}
}

namespace std
{
	template<typename _Ty>
	struct hash<Mahakam::Asset<_Ty>>
	{
		size_t operator()(const Mahakam::Asset<_Ty>& _Keyval) const noexcept
		{
			return hash<_Ty*>()(_Keyval.Get().get());
		}
	};
}