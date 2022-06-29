#include "mhpch.h"
#include "AssetDatabase.h"

#include "Mahakam/Core/Utility.h"

#include "AssetImporter.h"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <random>
#include <fstream>

namespace Mahakam
{
	static std::random_device rd;
	static std::default_random_engine generator(rd());
	static std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	//void AssetDatabase::RegisterAssetImporter(const std::string& extension, Ref<AssetImporter> assetImport)
	MH_DEFINE_FUNC(AssetDatabase::RegisterAssetImporter, void, const std::string& extension, Ref<AssetImporter> assetImporter)
	{
		m_AssetImporters[extension] = assetImporter;
		m_AssetImporters[assetImporter->GetImporterProps().Extension] = assetImporter;
	};

	//void AssetDatabase::DeregisterAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAssetImporter, void, const std::string& extension)
	{
		auto iter = m_AssetImporters.find(extension);
		if (iter != m_AssetImporters.end())
		{
			std::string extension = iter->second->GetImporterProps().Extension;
			long useCount = iter->second.use_count();

			m_AssetImporters.erase(iter);

			// 2 references should exist at this point, the extension itself and this iterator
			if (useCount <= 2)
				m_AssetImporters.erase(extension);
		}
	};

	//void AssetDatabase::DeregisterAllAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAllAssetImporters, void) // TODO: Define the function
	{
		
	};

	//Ref<AssetImporter> AssetDatabase::GetAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporter, Ref<AssetImporter>, const std::string& extension)
	{
		auto iter = m_AssetImporters.find(extension);
		if (iter != m_AssetImporters.end())
			return iter->second;

		return nullptr;
	};

	//const AssetDatabase::ImporterMap& AssetDatabase::GetAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporters, const AssetDatabase::ImporterMap&)
	{
		return m_AssetImporters;
	};

	//void AssetDatabase::ReloadAsset(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::ReloadAsset, void, uint64_t id)
	{
		auto pathIter = m_AssetPaths.find(id);
		if (pathIter == m_AssetPaths.end())
			RefreshAssetImports();

		auto iter = m_CachedAssets.find(id);
		if (iter != m_CachedAssets.end())
		{
			iter->second.Asset = nullptr;
			Ref<void> asset = LoadAssetFromID(id);
			iter->second.Asset = asset;
		}
	};

	//void AssetDatabase::ReloadAssetImports()
	MH_DEFINE_FUNC(AssetDatabase::ReloadAssetImports, void)
	{
		// Import any assets that haven't been imported yet
		// TODO: Think about whether this is needed? Maybe people don't want to import everything
		//RecursiveImportAssets(FileUtility::ASSET_PATH);

		// Recreate asset ID to filepath mapping
		RefreshAssetImports();

		// Reimport all imported assets
		for (auto& kv : m_CachedAssets)
		{
			kv.second.Asset = nullptr;
			Ref<void> asset = LoadAssetFromID(kv.first);
			kv.second.Asset = asset;
		}
	};

	//void AssetDatabase::RefreshAssetImports()
	MH_DEFINE_FUNC(AssetDatabase::RefreshAssetImports, void)
	{
		m_AssetPaths.clear();
		RecursiveCacheAssets(FileUtility::IMPORT_PATH);
	};

	//std::filesystem::path AssetDatabase::GetAssetImportPath(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImportPath, std::filesystem::path, uint64_t id)
	{
		auto iter = m_AssetPaths.find(id);
		if (iter != m_AssetPaths.end())
			return iter->second;

		return "";
	};

	//const AssetDatabase::AssetMap& AssetDatabase::GetAssetHandles()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetHandles, const AssetDatabase::AssetMap&)
	{
		return m_AssetPaths;
	};

	//uint32_t AssetDatabase::GetAssetReferences(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetReferences, uint32_t, uint64_t id)
	{
		auto iter = m_CachedAssets.find(id);
		if (iter != m_CachedAssets.end())
			return iter->second.UseCount;

		return 0;
	};

	//uint32_t AssetDatabase::GetStrongReferences(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::GetStrongReferences, uint32_t, uint64_t id)
	{
		auto iter = m_CachedAssets.find(id);
		if (iter != m_CachedAssets.end())
			return iter->second.Asset.use_count();

		return 0;
	};

	//std::string GetAssetType::GetAssetType(const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::ReadAssetInfo, AssetDatabase::AssetInfo, const std::filesystem::path& importPath)
	{
		if (std::filesystem::is_directory(importPath) || !std::filesystem::exists(importPath))
		{
			MH_CORE_WARN("The path {0} doesn't exist", importPath.string());
			return {};
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(importPath.string());
		}
		catch (YAML::ParserException e)
		{
			MH_CORE_WARN("Weird yaml file found in {0}: {1}", importPath.string(), e.msg);
		}

		AssetInfo info;

		YAML::Node idNode = data["ID"];
		if (idNode)
			info.ID = idNode.as<uint64_t>();

		YAML::Node filepathNode = data["Filepath"];
		if (filepathNode)
		{
			char seperator = std::filesystem::path::preferred_separator;

			std::string filepathUnix = filepathNode.as<std::string>();
			std::replace(filepathUnix.begin(), filepathUnix.end(), '/', seperator);

			info.Filepath = filepathUnix;
		}

		YAML::Node extensionNode = data["Extension"];
		if (extensionNode)
			info.Extension = extensionNode.as<std::string>();

		return info;
	};

	//uint64_t AssetDatabase::SaveAsset(Ref<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::SaveAsset, uint64_t, Ref<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		std::string extension = filepath.extension().string();

		auto iter = m_AssetImporters.find(extension);
		if (iter != m_AssetImporters.end())
		{
			// Read the ID if it exists, otherwise generate
			uint64_t id = 0;
			if (std::filesystem::exists(importPath))
				id = ReadAssetInfo(importPath).ID;
			while (id == 0) // TODO: Check if this ID already exists
				id = distribution(generator);

			FileUtility::CreateDirectories(importPath.parent_path());

			YAML::Emitter emitter;
			emitter << YAML::BeginMap;

			char seperator = std::filesystem::path::preferred_separator;

			std::string filepathUnix = filepath.string();
			std::replace(filepathUnix.begin(), filepathUnix.end(), seperator, '/');

			emitter << YAML::Key << "Filepath";
			emitter << YAML::Value << filepathUnix;
			emitter << YAML::Key << "Extension";
			emitter << YAML::Value << m_AssetImporters[extension]->GetImporterProps().Extension;
			emitter << YAML::Key << "ID";
			emitter << YAML::Value << id;

			iter->second->Serialize(emitter, asset);

			emitter << YAML::EndMap;

			std::ofstream filestream(importPath);
			filestream << emitter.c_str();

			return id;
		}

		return 0;
	};

	//Ref<void> AssetDatabase::LoadAssetFromID(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::LoadAssetFromID, Ref<void>, uint64_t id)
	{
		// If the asset has already been loaded, just return it
		auto cacheIter = m_CachedAssets.find(id);
		if (cacheIter != m_CachedAssets.end() && cacheIter->second.Asset)
			return cacheIter->second.Asset;

		// Find the import path from the ID
		std::filesystem::path importPath;
		auto iter = m_AssetPaths.find(id);
		if (iter != m_AssetPaths.end())
			importPath = iter->second;

		if (!std::filesystem::exists(importPath))
		{
			MH_CORE_WARN("The path {0} doesn't exist", importPath.string());
			return nullptr;
		}

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(importPath.string());
		}
		catch (YAML::ParserException e)
		{
			MH_CORE_WARN("AssetDatabase encountered exception trying to import yaml file {0}: {1}", importPath.string(), e.msg);
		}

		// If the asset type has an importer
		std::string extension = data["Extension"].as<std::string>();
		auto importIter = m_AssetImporters.find(extension);
		if (importIter != m_AssetImporters.end())
		{
			// TODO: Make the importer use Ref instead
			Asset<void> asset = importIter->second->Deserialize(data);

			return asset.Get();
		}

		return nullptr;
	};

	//void AssetDatabase::RegisterAsset(uint64_t id, Ref<void> asset)
	MH_DEFINE_FUNC(AssetDatabase::RegisterAsset, void, uint64_t id)
	{
		if (id)
		{
			auto iter = m_CachedAssets.find(id);
			if (iter != m_CachedAssets.end())
				iter->second.UseCount++;
			else
			{
				Ref<void> asset = AssetDatabase::LoadAssetFromID(id);
				m_CachedAssets[id] = { asset, 1 };
			}
		}
	};

	//void AssetDatabase::DeregisterAsset(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAsset, void, uint64_t id)
	{
		if (id)
		{
			auto iter = m_CachedAssets.find(id);
			if (iter != m_CachedAssets.end())
			{
				iter->second.UseCount--;

				if (iter->second.UseCount <= 0)
					m_CachedAssets.erase(iter);
			}
		}
	};

	void AssetDatabase::RecursiveCacheAssets(const std::filesystem::path& filepath)
	{
		auto iter = std::filesystem::directory_iterator(filepath);

		for (auto& directory : iter)
		{
			if (directory.is_directory())
			{
				RecursiveCacheAssets(directory.path());
			}
			else if (directory.path().extension() == ".import")
			{
				uint64_t id = ReadAssetInfo(directory.path()).ID;

				if (id)
				{
					char seperator = std::filesystem::path::preferred_separator;

					std::string filepathUnix = directory.path().string();
					std::replace(filepathUnix.begin(), filepathUnix.end(), '/', seperator);

					m_AssetPaths[id] = filepathUnix;
				}
			}
		}
	}

	void AssetDatabase::RecursiveImportAssets(const std::filesystem::path& filepath)
	{
		auto iter = std::filesystem::directory_iterator(filepath);

		for (auto& directory : iter)
		{
			if (directory.is_directory())
			{
				RecursiveImportAssets(directory.path());
			}
			else
			{
				Ref<AssetImporter> importer = GetAssetImporter(directory.path().extension().string());
				if (importer)
				{
					std::filesystem::path importPath = FileUtility::GetImportPath(directory.path());

					if (!std::filesystem::exists(importPath))
					{
						YAML::Node node;
						importer->OnWizardOpen(node);

						Asset<void> asset(importPath);

						importer->OnWizardImport(asset, directory.path(), importPath);
					}
				}
			}
		}
	}
}