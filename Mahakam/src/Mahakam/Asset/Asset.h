#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Core/AssetDatabase.h"

namespace Mahakam
{
	template<typename T>
	class Asset
	{
	private:
		uint64_t m_ID = 0;

		Ref<T> m_Ptr = nullptr;

	public:
		Asset(Ref<T> ptr)
		{
			m_Ptr = ptr;
		}

		Asset(uint64_t id) : m_ID(id) { }

		Asset(const std::filesystem::path& filepath)
		{
			m_ID = AssetDatabase::GetAssetInfo(filepath).ID;
		}

		Ref<T> Get()
		{
			if (m_Ptr)
				return m_Ptr;
			return AssetDatabase::LoadAsset<T>(m_ID);
		}

		Ref<T> operator->() const noexcept
		{
			return Get();
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
}