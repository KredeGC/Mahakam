#pragma once

#include "Core.h"
#include <string>

namespace Mahakam
{
	class AssetDatabase
	{
	public:
		template<typename T>
		static void saveAsset(const std::string& filepath, const Ref<T>& asset)
		{
			uint64_t id = rand();

			int length;
			const void* data = asset->serialize(length);

			// Save to file along with id

			delete[] data;
		}
	};
}