#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/SharedLibrary.h"
#include "Mahakam/Core/Types.h"

#include <filesystem>
#include <string>

namespace Mahakam
{
	class ResourceImporter;

	class ResourceRegistry
	{
	public:
		typedef uint64_t AssetID; // TODO: Use AssetDatabase::AssetID

		using ExtensionMap = std::unordered_multimap<std::string, Ref<ResourceImporter>>;
		using ExtensionIter = std::pair<ExtensionMap::const_iterator, ExtensionMap::const_iterator>;

		using ImporterMap = UnorderedMap<std::string, Ref<ResourceImporter>>;

	private:
		inline static ExtensionMap s_AssetExtensions;
		inline static ImporterMap s_AssetImporters;

	public:
		void RegisterAssetImporter(const std::string& extension, Ref<ResourceImporter> assetImport);
		void DeregisterAssetImporter(const std::string& extension);
		void DeregisterAllAssetImporters();
		Ref<ResourceImporter> GetAssetImporter(const std::string& extension);
		const ImporterMap& GetAssetImporters();
		ExtensionIter GetAssetImporterExtension(const std::string& extension);
		void RegisterDefaultAssetImporters();
		void DeregisterDefaultAssetImporters();
	};
}