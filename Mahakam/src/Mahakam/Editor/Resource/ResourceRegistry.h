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

		struct ImportInfo
		{
			AssetID ID = 0;
			std::filesystem::path Filepath;
			std::string Type;
		};

		using AssetMap = UnorderedMap<AssetID, ImportInfo>;

		using ExtensionMap = std::unordered_multimap<std::string, Ref<ResourceImporter>>;
		using ExtensionIter = std::pair<ExtensionMap::const_iterator, ExtensionMap::const_iterator>;

		using ImporterMap = UnorderedMap<std::string, Ref<ResourceImporter>>;

		inline static const std::string ImportExtension = ".import";

	private:
		inline static AssetMap s_ImportPaths;

		inline static ExtensionMap s_AssetExtensions;
		inline static ImporterMap s_AssetImporters;

	public:
		static void RegisterAssetImporter(const std::string& extension, Ref<ResourceImporter> assetImport);
		static void DeregisterAssetImporter(const std::string& extension);
		static void DeregisterAllAssetImporters();
		static Ref<ResourceImporter> GetAssetImporter(const std::string& extension);
		static const ImporterMap& GetAssetImporters();
		static ExtensionIter GetAssetImporterExtension(const std::string& extension);
		static void RegisterDefaultAssetImporters();
		static void DeregisterDefaultAssetImporters();

		static void RefreshImportPaths();

		static const AssetMap& GetImports() { return s_ImportPaths; }

		static ImportInfo GetImportInfo(const std::filesystem::path& filepath);

	private:
		static void RecursiveCacheAssets(const std::filesystem::path& filepath);
	};
}