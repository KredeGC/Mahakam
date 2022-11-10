#pragma once

#include "AssetDatabase.h"

#include "Mahakam/Core/Allocator.h"

#include <cstddef>
#include <filesystem>
#include <functional>
#include <memory>
#include <type_traits>

namespace Mahakam
{
	class AssetDatabase;

	template<typename T>
	class Asset
	{
	private:
		template<typename T2>
		friend class Asset;

		friend class AssetDatabase;

		using AssetID = AssetDatabase::AssetID;
		using ControlBlock = AssetDatabase::ControlBlock;

		ControlBlock* m_Control;

	public:
		Asset() :
			m_Control(nullptr) {}

		Asset(const std::nullptr_t&) :
			m_Control(nullptr) {}

		explicit Asset(T* value, const std::function<void(void*)>& deleter)
		{
			m_Control = Allocator::Allocate<ControlBlock>(1);
			Allocator::Construct<ControlBlock>(m_Control);

			m_Control->UseCount = 1;
			m_Control->ID = 0;
			m_Control->Ptr = value;

			m_Control->DeleteData = deleter;
		}

		explicit Asset(AssetID id)
		{
			// Register if the ID is valid
			if (id)
				m_Control = AssetDatabase::IncrementAsset(id);
			else
				m_Control = nullptr;
		}

		explicit Asset(ControlBlock* control)
			: m_Control(control)
		{
			// Increment the count
			IncrementRef();
		}

		explicit Asset(const std::filesystem::path& importPath)
		{
			// Register if the ID is valid
			AssetID id = AssetDatabase::ReadAssetInfo(importPath).ID;
			if (id)
				m_Control = AssetDatabase::IncrementAsset(id);
			else
				m_Control = nullptr;
		}

#pragma region Copy & Move constructors
		Asset(const Asset& other) noexcept :
			m_Control(other.m_Control)
		{
			// Increment the count
			IncrementRef();
		}

		Asset(Asset&& other) noexcept :
			m_Control(other.m_Control)
		{
			// Invalidate the other, as it's destructor is still called
			other.m_Control = nullptr;
		}

		template<typename T2>
		Asset(const Asset<T2>& other) noexcept :
			m_Control(other.m_Control)
		{
			// Increment the count
			IncrementRef();
		}

		template<typename T2>
		Asset(Asset<T2>&& other) noexcept :
			m_Control(other.m_Control)
		{
			// Invalidate the other, as it's destructor is still called
			other.m_Control = nullptr;
		}
#pragma endregion

		~Asset()
		{
			// Remember to clear on delete
			DecrementRef();
		}

		template<typename T2>
		explicit operator Asset<T2>() const
		{
			return Asset<T2>(m_Control);
		}

#pragma region Copy & Move operators
		Asset& operator=(const Asset& rhs)
		{
			// Remember to clear if we already have something
			DecrementRef();

			m_Control = rhs.m_Control;

			// Register our new ID
			IncrementRef();
			return *this;
		}

		Asset& operator=(Asset&& rhs) noexcept
		{
			// Remember to clear if we already have something
			DecrementRef();

			m_Control = rhs.m_Control;
			// Invalidate the rhs, as it's destructor is still called
			rhs.m_Control = nullptr;
			return *this;
		}

		template<typename T2>
		Asset& operator=(const Asset<T2>& rhs)
		{
			// Remember to clear if we already have something
			DecrementRef();

			m_Control = rhs.m_Control;

			// Register our new ID
			IncrementRef();
			return *this;
		}

		template<typename T2>
		Asset& operator=(Asset<T2>&& rhs) noexcept
		{
			// Remember to clear if we already have something
			DecrementRef();

			m_Control = rhs.m_Control;
			// Invalidate the rhs, as it's destructor is still called
			rhs.m_Control = nullptr;
			return *this;
		}

		Asset& operator=(const std::nullptr_t& rhs)
		{
			// Remember to clear if we already have something
			DecrementRef();

			m_Control = nullptr;
			return *this;
		}
#pragma endregion

		void Save(const std::filesystem::path& filepath, const std::filesystem::path& importPath)
		{
			m_Control = AssetDatabase::SaveAsset(m_Control, filepath, importPath);
		}

		AssetID GetID() const
		{
			return m_Control ? m_Control->ID : 0;
		}

		std::filesystem::path GetImportPath() const
		{
			if (m_Control)
				return AssetDatabase::GetAssetImportPath(m_Control->ID);
			return "";
		}

		size_t UseCount() const noexcept
		{
			return m_Control ? m_Control->UseCount : 0;
		}

		T* get() const noexcept
		{
			return m_Control ? static_cast<T*>(m_Control->Ptr) : nullptr;
		}

		template<typename Ty = T, std::enable_if_t<!std::is_void<Ty>::value, bool> = true>
		Ty& operator*() const noexcept
		{
			return *static_cast<T*>(m_Control->Ptr);
		}

		T* operator->() const noexcept
		{
			return m_Control ? static_cast<T*>(m_Control->Ptr) : nullptr;
		}

		explicit operator bool() const noexcept
		{
			return m_Control;
		}

	private:
		void IncrementRef()
		{
			if (m_Control)
				m_Control->UseCount++;
		}

		void DecrementRef()
		{
			if (!m_Control) return;

			if (--m_Control->UseCount == 0)
			{
				if (m_Control->ID)
				{
					AssetDatabase::UnloadAsset(m_Control);
				}
				else
				{
					auto destroy = m_Control->DeleteData;
					if (destroy)
						destroy(m_Control->Ptr);

					Allocator::Deconstruct<ControlBlock>(m_Control);
					Allocator::Deallocate<ControlBlock>(m_Control, 1);
				}
			}
		}
	};

	template<typename T, typename ... Args>
	constexpr Asset<T> CreateAsset(Args&& ... args)
	{
		T* value = Allocator::Allocate<T>(1);
		Allocator::Construct<T>(value, std::forward<Args>(args)...);

		auto deleter = [](void* p)
		{
			Allocator::Deconstruct<T>(static_cast<T*>(p));
			Allocator::Deallocate<T>(static_cast<T*>(p), 1);
		};

		return Asset<T>(value, deleter);
	}

	template<typename T1, typename T2>
	bool operator==(const Asset<T1>& lhs, const Asset<T2>& rhs) noexcept
	{
		return lhs.get() == rhs.get();
	}
}

namespace std
{
	template<typename T>
	struct hash<::Mahakam::Asset<T>>
	{
		size_t operator()(const ::Mahakam::Asset<T>& kv) const noexcept
		{
			return hash<T*>()(kv.get());
		}
	};
}