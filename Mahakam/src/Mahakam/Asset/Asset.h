#pragma once

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Log.h"

#include "AssetDatabase.h"
#include "AssetDataFunctions.h"

#include <cstddef>
#include <filesystem>
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

		using ExtensionType = AssetDatabase::ExtensionType;

		ControlBlock* m_Control;

		template<typename T2>
		static constexpr bool IsBaseOrVoid = std::is_void_v<T> || std::is_void_v<T2> || std::is_base_of_v<T, T2> || std::is_base_of_v<T2, T>;

	public:
		Asset() :
			m_Control(nullptr) {}

		Asset(const std::nullptr_t&) :
			m_Control(nullptr) {}

		explicit Asset(ControlBlock* control) :
			m_Control(control)
		{
			// Increment the count
			IncrementRef();
		}

		explicit Asset(AssetID id)
		{
			// Register if the ID is valid
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

		template<typename T2, typename = std::enable_if_t<IsBaseOrVoid<T2>>>
		Asset(const Asset<T2>& other) noexcept :
			m_Control(other.m_Control)
		{
			// Increment the count
			IncrementRef();
		}

		template<typename T2, typename = std::enable_if_t<IsBaseOrVoid<T2>>>
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

		template<typename T2, typename = std::enable_if_t<IsBaseOrVoid<T2>>>
		Asset& operator=(const Asset<T2>& rhs)
		{
			// Remember to clear if we already have something
			DecrementRef();

			m_Control = rhs.m_Control;

			// Register our new ID
			IncrementRef();
			return *this;
		}

		template<typename T2, typename = std::enable_if_t<IsBaseOrVoid<T2>>>
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

		void Save(AssetID id, const ExtensionType& extension)
		{
			ControlBlock* control = AssetDatabase::SaveAsset(m_Control, id, extension);

			// If the control block is changed, we might need to remove the old one
			if (control != m_Control)
				DecrementRef();

			m_Control = control;
		}

		AssetID GetID() const
		{
			return m_Control ? m_Control->ID : 0;
		}

		size_t UseCount() const noexcept
		{
			return m_Control ? m_Control->UseCount : 0;
		}

		AssetState GetState() const noexcept
		{
			return m_Control ? m_Control->State : AssetState::Failed;
		}

		T* get() const noexcept
		{
			return m_Control ? reinterpret_cast<T*>(m_Control + 1) : nullptr;
		}

		ControlBlock* leak() noexcept
		{
			ControlBlock* ptr = m_Control;
			m_Control = nullptr;

			return m_Control;
		}

		template<typename Ty = T, typename = std::enable_if_t<!std::is_void<Ty>::value>>
		Ty& operator*() const noexcept
		{
			return *reinterpret_cast<T*>(m_Control + 1);
		}

		T* operator->() const noexcept
		{
			return m_Control ? reinterpret_cast<T*>(m_Control + 1) : nullptr;
		}

		explicit operator bool() const noexcept
		{
			return m_Control ? (m_Control->State == AssetState::Loaded || m_Control->State == AssetState::Streaming) : false;
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
				// Remember to erase it from AssetDatabase
				if (m_Control->ID)
					AssetDatabase::UnloadAsset(m_Control);

				auto destroy = m_Control->Functions->Delete;

				MH_ASSERT(destroy, "Asset destructor encountered invalid control block");

				destroy(m_Control);
			}
		}
	};

	template<typename T, typename ... Args>
	constexpr Asset<T> CreateAsset(Args&& ... args)
	{
		DataBlock<T>* block = reinterpret_cast<DataBlock<T>*>(GetAssetDataFunctions<T>()->CreateControlBlock());

		Allocator::Construct(&block->Data, std::forward<Args>(args)...);
		block->Control.State = AssetState::Loaded;

		return Asset<T>(&block->Control);
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