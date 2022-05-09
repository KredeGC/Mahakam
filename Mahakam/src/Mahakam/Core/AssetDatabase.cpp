#include "mhpch.h"
#include "AssetDatabase.h"

#include <random>

namespace Mahakam
{
	static std::random_device rd;
	static std::default_random_engine generator(rd());
	static std::uniform_int_distribution<uint64_t> distribution(0, 0xFFFFFFFFFFFFFFFF);

	//void AssetDatabase::RegisterAssetImporter(const std::string& extension, Ref<AssetImporter> assetImport)
	MH_DEFINE_FUNC(AssetDatabase::RegisterAssetImporter, void, const std::string& extension, Ref<AssetImporter> assetImport)
	{
		m_AssetImporters[extension] = assetImport;
	};

	//void AssetDatabase::DeregisterAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAssetImporter, void, const std::string& extension)
	{
		m_AssetImporters.erase(extension);
	};

	//Ref<AssetImporter> AssetDatabase::GetAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporter, Ref<AssetImporter>, const std::string& extension)
	{
		auto iter = m_AssetImporters.find(extension);
		if (iter != m_AssetImporters.end())
			return iter->second;

		return nullptr;
	};

	//void AssetDatabase::CollectGarbage()
	MH_DEFINE_FUNC(AssetDatabase::CollectGarbage, void)
	{
		// Remove any unused references, so they don't clog up memory
		auto iter = m_AssetIndex.begin();
		while (iter != m_AssetIndex.end())
		{
			auto assetIter = m_CachedAssets.find(iter->second);
			if (assetIter == m_CachedAssets.end() || assetIter->second.expired())
			{
				if (assetIter != m_CachedAssets.end())
					m_CachedAssets.erase(assetIter);
				iter = m_AssetIndex.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	};

	//void AssetDatabase::ReloadAssetImports()
	MH_DEFINE_FUNC(AssetDatabase::ReloadAssetImports, void)
	{
		CollectGarbage();

		// Import any assets that haven't been imported yet
		// TODO: Think about whether this is need? Maybe people don't want to import everything
		//RecursiveImportAssets("assets");

		// Recreate asset ID to filepath mapping
		m_AssetPaths.clear();
		RecursiveCacheAssets("res");
	};

	//AssetDatabase::AssetMap AssetDatabase::GetAssetHandles()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetHandles, AssetDatabase::AssetMap)
	{
		return m_AssetPaths;
	};

	//uint32_t AssetDatabase::GetAssetReferences(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetReferences, uint32_t, uint64_t id)
	{
		auto cacheIter = m_CachedAssets.find(id);
		if (cacheIter != m_CachedAssets.end())
			return cacheIter->second.use_count();

		return 0;
	};

	//uint64_t AssetDatabase::GetAssetID(Ref<void> asset)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetID, uint64_t, Ref<void> asset)
	{
		auto iter = m_AssetIndex.find(asset.get());
		if (iter != m_AssetIndex.end())
			return iter->second;

		return 0;
	};

	//std::string GetAssetType::GetAssetType(const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetInfo, AssetDatabase::AssetInfo, const std::filesystem::path& importPath)
	{
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

		info.ID = data["ID"].as<uint64_t>();
		info.Filepath = data["Filepath"].as<std::string>();
		info.Extension = data["Extension"].as<std::string>();

		return info;
	};

	//void AssetDatabase::SaveAsset(Ref<void> asset, const std::string& extension, const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::SaveAsset, void, Ref<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		std::string extension = filepath.extension().string();

		auto iter = m_AssetImporters.find(extension);
		if (iter != m_AssetImporters.end())
		{
			// Read the ID if it exists, otherwise generate
			uint64_t id = 0;
			if (std::filesystem::exists(importPath))
				id = ReadAssetID(importPath);
			if (id == 0) // TODO: Check if this ID already exists
				id = distribution(generator);

			FileUtility::CreateDirectories(importPath.parent_path());

			YAML::Emitter emitter;
			emitter << YAML::BeginMap;

			emitter << YAML::Key << "Filepath";
			emitter << YAML::Value << filepath.string();
			emitter << YAML::Key << "Extension";
			emitter << YAML::Value << extension;
			emitter << YAML::Key << "ID";
			emitter << YAML::Value << id;

			iter->second->Serialize(emitter, asset);

			emitter << YAML::EndMap;

			std::ofstream filestream(importPath);
			filestream << emitter.c_str();
		}
	};

	//Ref<void> AssetDatabase::LoadAsset(const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::LoadAssetFromPath, Ref<void>, const std::filesystem::path& importPath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(importPath.string());
		}
		catch (YAML::ParserException e)
		{
			MH_CORE_WARN("Weird yaml file found in {0}: {1}", importPath.string(), e.msg);
		}

		// If the asset has already been loaded, just return it
		uint64_t id = data["ID"].as<uint64_t>();
		auto cacheIter = m_CachedAssets.find(id);
		if (cacheIter != m_CachedAssets.end())
		{
			if (Ref<void> asset = cacheIter->second.lock())
				return asset;
		}

		// If the asset type has an importer
		std::string extension = data["Extension"].as<std::string>();
		auto importIter = m_AssetImporters.find(extension);
		if (importIter != m_AssetImporters.end())
		{
			Ref<void> asset = importIter->second->Deserialize(data);

			m_CachedAssets[id] = asset;
			m_AssetIndex[asset.get()] = id;

			return asset;
		}

		return nullptr;
	};

	//Ref<void> AssetDatabase::LoadAsset(uint64_t id)
	MH_DEFINE_FUNC(AssetDatabase::LoadAssetFromID, Ref<void>, uint64_t id)
	{
		auto iter = m_AssetPaths.find(id);
		if (iter != m_AssetPaths.end())
			return LoadAsset(iter->second);

		return nullptr;
	};

	uint64_t AssetDatabase::ReadAssetID(const std::filesystem::path& importPath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(importPath.string());
		}
		catch (YAML::ParserException e)
		{
			MH_CORE_WARN("Weird yaml file found in {0}: {1}", importPath.string(), e.msg);
		}

		return data["ID"].as<uint64_t>();
	}

	void AssetDatabase::RecursiveCacheAssets(const std::filesystem::path& filepath)
	{
		auto iter = std::filesystem::directory_iterator(filepath);

		for (auto& directory : iter)
		{
			if (directory.is_directory())
			{
				RecursiveCacheAssets(directory.path());
			}
			else if (directory.path().extension() == ".yaml" || directory.path().extension() == ".yml")
			{
				uint64_t id = ReadAssetID(directory.path());

				if (id)
					m_AssetPaths[id] = directory.path().string();
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
						Ref<void> asset = AssetDatabase::LoadAsset(importPath);
						importer->OnWizardImport(asset, directory.path());

						AssetDatabase::SaveAsset(asset, directory.path(), importPath);
					}
				}
			}
		}
	}
}