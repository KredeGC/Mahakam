#include "Mahakam/mhpch.h"
#include "AssetDatabase.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Random.h"

#include "AssetImporter.h"
#include "MaterialAssetImporter.h"
#include "MeshAssetImporter.h"
#include "ShaderAssetImporter.h"
#include "SoundAssetImporter.h"
#include "TextureAssetImporter.h"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <fstream>

namespace Mahakam
{
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
		m_AssetImporters.clear();
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

	//void AssetDatabase::LoadDefaultAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::LoadDefaultAssetImporters, void)
	{
		// Material
		Ref<MaterialAssetImporter> materialAssetImporter = CreateRef<MaterialAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".material", materialAssetImporter);

		// Mesh
		Ref<MeshAssetImporter> meshAssetImporter = CreateRef<MeshAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".gltf", meshAssetImporter);
		AssetDatabase::RegisterAssetImporter(".glb", meshAssetImporter);

		// Shader
		Ref<ShaderAssetImporter> shaderAssetImporter = CreateRef<ShaderAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".shader", shaderAssetImporter);

		// Sound
		Ref<SoundAssetImporter> soundAssetImporter = CreateRef<SoundAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".wav", soundAssetImporter);
		AssetDatabase::RegisterAssetImporter(".mp3", soundAssetImporter);

		// Texture
		Ref<TextureAssetImporter> textureAssetImporter = CreateRef<TextureAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".png", textureAssetImporter);
		AssetDatabase::RegisterAssetImporter(".jpeg", textureAssetImporter);
		AssetDatabase::RegisterAssetImporter(".jpg", textureAssetImporter);
		AssetDatabase::RegisterAssetImporter(".hdr", textureAssetImporter);
	};

	//void AssetDatabase::UnloadDefaultAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::UnloadDefaultAssetImporters, void)
	{
		// Material
		AssetDatabase::DeregisterAssetImporter(".material");

		// Mesh
		AssetDatabase::DeregisterAssetImporter(".gltf");
		AssetDatabase::DeregisterAssetImporter(".glb");

		// Shader
		AssetDatabase::DeregisterAssetImporter(".shader");

		// Sound
		AssetDatabase::DeregisterAssetImporter(".wav");
		AssetDatabase::DeregisterAssetImporter(".mp3");

		// Texture
		AssetDatabase::DeregisterAssetImporter(".png");
		AssetDatabase::DeregisterAssetImporter(".jpeg");
		AssetDatabase::DeregisterAssetImporter(".jpg");
		AssetDatabase::DeregisterAssetImporter(".hdr");
	};

	//void AssetDatabase::ReloadAsset(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::ReloadAsset, void, AssetDatabase::AssetID id)
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

	//std::filesystem::path AssetDatabase::GetAssetImportPath(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImportPath, std::filesystem::path, AssetDatabase::AssetID id)
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

	//uint32_t AssetDatabase::GetAssetReferences(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetReferences, uint32_t, AssetDatabase::AssetID id)
	{
		auto iter = m_CachedAssets.find(id);
		if (iter != m_CachedAssets.end())
			return iter->second.UseCount;

		return 0;
	};

	//uint32_t AssetDatabase::GetStrongReferences(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::GetStrongReferences, uint32_t, AssetDatabase::AssetID id)
	{
		auto iter = m_CachedAssets.find(id);
		if (iter != m_CachedAssets.end())
			return iter->second.Asset.use_count();

		return 0;
	};

	//std::string GetAssetType::GetAssetType(const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::ReadAssetInfo, AssetDatabase::AssetInfo, const std::filesystem::path& importPath)
	{
		if (!std::filesystem::exists(importPath) || std::filesystem::is_directory(importPath))
		{
			MH_CORE_WARN("AssetDatabase::ReadAssetInfo: The path '{0}' doesn't exist", importPath.string());
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

	//AssetDatabase::AssetID AssetDatabase::SaveAsset(Ref<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::SaveAsset, AssetDatabase::AssetID, Ref<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		// Read asset info and ID
		std::string extension;
		AssetID id = 0;
		if (std::filesystem::exists(importPath))
		{
			AssetInfo info = ReadAssetInfo(importPath);
			extension = info.Extension;
			id = info.ID;
		}
		else
		{
			extension = filepath.extension().string();
			id = Random::GetRandomID64();
		}

		auto iter = m_AssetImporters.find(extension);
		if (iter != m_AssetImporters.end())
		{
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

	//Ref<void> AssetDatabase::LoadAssetFromID(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::LoadAssetFromID, Ref<void>, AssetDatabase::AssetID id)
	{
		MH_CORE_ASSERT(id, "Asset ID to be loaded cannot be 0");

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
			MH_CORE_WARN("AssetDatabase::LoadAssetFromID: The path '{0}' doesn't exist", importPath.string());
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
			return importIter->second->Deserialize(data);
		}

		return nullptr;
	};

	//void AssetDatabase::RegisterAsset(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::RegisterAsset, void, AssetDatabase::AssetID id)
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

	//void AssetDatabase::DeregisterAsset(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAsset, void, AssetDatabase::AssetID id)
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
				AssetID id = ReadAssetInfo(directory.path()).ID;

				if (id)
				{
					char seperator = std::filesystem::path::preferred_separator;

					std::string filepathUnix = directory.path().string();
					std::replace(filepathUnix.begin(), filepathUnix.end(), '/', seperator);

					auto iter = m_AssetPaths.find(id);

					MH_CORE_ASSERT(iter == m_AssetPaths.end(), "AssetDatabase::RecursiveCacheAssets: Asset with the given ID already exists");

					m_AssetPaths[id] = filepathUnix;
				}
			}
		}
	}

	void AssetDatabase::RecursiveImportAssets(const std::filesystem::path& filepath)
	{
		/*auto iter = std::filesystem::directory_iterator(filepath);

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
						importer->OnWizardOpen(directory.path(), node);

						Asset<void> asset(importPath);

						importer->OnWizardImport(asset, directory.path(), importPath);
					}
				}
			}
		}*/
	}
}