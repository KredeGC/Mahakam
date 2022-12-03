#include "Mahakam/mhpch.h"
#include "AssetDatabase.h"

#include "AssetImporter.h"
#include "MaterialAssetImporter.h"
#include "MeshAssetImporter.h"
#include "ShaderAssetImporter.h"
#include "SoundAssetImporter.h"
#include "TextureAssetImporter.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Random.h"

#include "Mahakam/Serialization/YAMLGuard.h"

#include <ryml/rapidyaml-0.4.1.hpp>

#include <algorithm>
#include <fstream>

namespace Mahakam
{
	//void AssetDatabase::RegisterAssetImporter(const std::string& extension, Ref<AssetImporter> assetImport)
	MH_DEFINE_FUNC(AssetDatabase::RegisterAssetImporter, void, const std::string& extension, Ref<AssetImporter> assetImporter)
	{
		s_AssetImporters.insert(assetImporter);
		s_AssetExtensions[extension] = assetImporter;
		s_AssetExtensions[assetImporter->GetImporterProps().Extension] = assetImporter;
	};

	//void AssetDatabase::DeregisterAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAssetImporter, void, const std::string& extension)
	{
		auto iter = s_AssetExtensions.find(extension);
		if (iter != s_AssetExtensions.end())
		{
			std::string extension = iter->second->GetImporterProps().Extension;
			long useCount = iter->second.use_count();

			s_AssetImporters.erase(iter->second);
			s_AssetExtensions.erase(iter);

			// 2 references should exist at this point, the extension itself and this iterator
			if (useCount <= 2)
				s_AssetExtensions.erase(extension);
		}
	};

	//void AssetDatabase::DeregisterAllAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAllAssetImporters, void)
	{
		s_AssetImporters.clear();
		s_AssetExtensions.clear();
	};

	//Ref<AssetImporter> AssetDatabase::GetAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporter, Ref<AssetImporter>, const std::string& extension)
	{
		auto iter = s_AssetExtensions.find(extension);
		if (iter != s_AssetExtensions.end())
			return iter->second;

		return nullptr;
	};

	//const AssetDatabase::ImporterSet& AssetDatabase::GetAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporters, const AssetDatabase::ImporterSet&)
	{
		return s_AssetImporters;
	};

	//const AssetDatabase::ImporterMap& AssetDatabase::GetAssetExtensions()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetExtensions, const AssetDatabase::ImporterMap&)
	{
		return s_AssetExtensions;
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
		auto pathIter = s_AssetPaths.find(id);
		if (pathIter == s_AssetPaths.end())
			RefreshAssetImports();

		// TODO: Is this even needed anymore? SaveAsset does something similar
		auto iter = s_LoadedAssets.find(id);
		if (iter != s_LoadedAssets.end())
		{
			// Delete our previous data
			auto destroy = iter->second->DeleteData;
			destroy(iter->second->Ptr);

			// Load the asset
			ControlBlock* control = LoadAndIncrementAsset(id);

			if (control)
			{
				// Move the pointer and destructor to the existing control block
				iter->second->Ptr = control->Ptr;
				iter->second->DeleteData = control->DeleteData;

				// Delete the control block
				Allocator::Deallocate<ControlBlock>(control, 1);
			}
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
		for (auto& kv : s_LoadedAssets)
		{
			// Delete our previous data
			auto destroy = kv.second->DeleteData;
			destroy(kv.second->Ptr);

			// Load the asset
			ControlBlock* control = LoadAndIncrementAsset(kv.first);

			if (control)
			{
				// Move the pointer and destructor to the existing control block
				kv.second->Ptr = control->Ptr;
				kv.second->DeleteData = control->DeleteData;

				// Delete the control block
				Allocator::Deallocate<ControlBlock>(control, 1);
			}
		}
	};

	//void AssetDatabase::RefreshAssetImports()
	MH_DEFINE_FUNC(AssetDatabase::RefreshAssetImports, void)
	{
		s_AssetPaths.clear();
		RecursiveCacheAssets(FileUtility::IMPORT_PATH);
	};

	//std::filesystem::path AssetDatabase::GetAssetImportPath(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImportPath, std::filesystem::path, AssetDatabase::AssetID id)
	{
		auto iter = s_AssetPaths.find(id);
		if (iter != s_AssetPaths.end())
			return iter->second;

		return "";
	};

	//const AssetDatabase::AssetMap& AssetDatabase::GetAssetHandles()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetHandles, const AssetDatabase::AssetMap&)
	{
		return s_AssetPaths;
	};

	//uint32_t AssetDatabase::GetAssetReferences(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetReferences, size_t, AssetDatabase::AssetID id)
	{
		auto iter = s_LoadedAssets.find(id);
		if (iter != s_LoadedAssets.end())
			return iter->second->UseCount;

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

		TrivialVector<char> buffer;

		if (!FileUtility::ReadFile(importPath, buffer))
			return {};

		try
		{
			ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(buffer.data(), buffer.size()));

			ryml::NodeRef root = tree.rootref();

			AssetInfo info;

			if (root.has_child("ID"))
				root["ID"] >> info.ID;

			if (root.has_child("Filepath"))
			{
				ryml::NodeRef filepathNode = root["Filepath"];
				char seperator = std::filesystem::path::preferred_separator;

				std::string filepathUnix;
				filepathNode >> filepathUnix;
				std::replace(filepathUnix.begin(), filepathUnix.end(), '/', seperator);

				info.Filepath = filepathUnix;
			}

			if (root.has_child("Extension"))
				root["Extension"] >> info.Extension;

			return info;
		}
		catch (std::runtime_error const& e)
		{
			MH_CORE_WARN("Weird yaml file found in {0}: {1}", importPath.string(), e.what());
		}

		return {};
	};

	//AssetDatabase::ControlBlock* AssetDatabase::SaveAsset(ControlBlock* control, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::SaveAsset, AssetDatabase::ControlBlock*, ControlBlock* control, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
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

		// If no importer exists with this extension
		auto iter = s_AssetExtensions.find(extension);
		if (iter == s_AssetExtensions.end())
			return nullptr;

		// Create the import directory, if it doesn't exist
		FileUtility::CreateDirectories(importPath.parent_path());

		// Serialize the asset
		char seperator = std::filesystem::path::preferred_separator;

		std::string filepathUnix = filepath.string();
		std::replace(filepathUnix.begin(), filepathUnix.end(), seperator, '/');

		ryml::Tree tree;

		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		root["Filepath"] << filepathUnix;
		root["Extension"] << iter->second->GetImporterProps().Extension;
		root["ID"] << id;

		Asset<void> asset(control);

		iter->second->Serialize(root, asset);

		std::ofstream filestream(importPath);
		filestream << tree;

		auto controlIter = s_LoadedAssets.find(id);
		if (controlIter != s_LoadedAssets.end())
		{
			ControlBlock* loadedControl = controlIter->second;

			// If the control block is different, then copy and invalidate
			if (loadedControl != control)
			{
				// Increment the UseCount
				loadedControl->UseCount++;

				// Delete our previous data
				auto destroy = loadedControl->DeleteData;
				destroy(loadedControl->Ptr);

				// Move the pointer and destructor to the existing control block
				loadedControl->Ptr = control->Ptr;
				loadedControl->DeleteData = std::move(control->DeleteData);

				control->Ptr = nullptr;
				control->DeleteData = nullptr;
			}

			return loadedControl;
		}
		else
		{
			control->ID = id;

			s_LoadedAssets[id] = control;

			return control;
		}
	};

	//AssetDatabase::ControlBlock* AssetDatabase::IncrementAsset(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::IncrementAsset, AssetDatabase::ControlBlock*, AssetDatabase::AssetID id)
	{
		MH_CORE_ASSERT(id, "Attempting to load an Asset with id 0");

		auto iter = s_LoadedAssets.find(id);
		if (iter != s_LoadedAssets.end())
		{
			iter->second->UseCount++;

			return iter->second;
		}
		else
		{
			ControlBlock* control = LoadAndIncrementAsset(id);

			if (control)
				s_LoadedAssets[id] = control;

			return control;
		}
	};

	//void AssetDatabase::DecrementAsset(ControlBlock* control)
	MH_DEFINE_FUNC(AssetDatabase::UnloadAsset, void, ControlBlock* control)
	{
		MH_CORE_ASSERT(control->UseCount == 0, "Attempting to unload multiple instances of Asset");

		s_LoadedAssets.erase(control->ID);
	};

	AssetDatabase::ControlBlock* AssetDatabase::LoadAndIncrementAsset(AssetID id)
	{
		MH_CORE_ASSERT(id, "Asset ID to be loaded cannot be 0");

		// Find the import path from the ID
		std::filesystem::path importPath;
		auto iter = s_AssetPaths.find(id);
		if (iter != s_AssetPaths.end())
			importPath = iter->second;

		if (!std::filesystem::exists(importPath))
		{
			MH_CORE_WARN("AssetDatabase::LoadAssetFromID: The path '{0}' doesn't exist", importPath.string());
			return nullptr;
		}

		std::ifstream ifs(importPath, std::ios::binary | std::ios::ate);

		if (!ifs)
			return nullptr;

		auto end = ifs.tellg();
		ifs.seekg(0, std::ios::beg);

		auto size = size_t(end - ifs.tellg());

		if (size == 0) // avoid undefined behavior 
			return nullptr;

		std::vector<char> buffer(size);

		if (!ifs.read(buffer.data(), buffer.size()))
			return nullptr;

		try
		{
			ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(buffer));

			ryml::NodeRef root = tree.rootref();

			// If the asset type has an importer
			std::string extension;
			if (root.has_child("Extension"))
				root["Extension"] >> extension;

			auto importIter = s_AssetExtensions.find(extension);
			if (importIter == s_AssetExtensions.end())
				return nullptr;

			// Deserialize the asset using the YAML node
			Asset<void> asset = importIter->second->Deserialize(root);

			if (!asset)
				return nullptr;

			asset.m_Control->ID = id;
			asset.IncrementRef();

			return asset.m_Control;
		}
		catch (std::runtime_error const& e)
		{
			MH_CORE_WARN("AssetDatabase encountered exception trying to import yaml file {0}: {1}", importPath.string(), e.what());
		}

		return nullptr;
	}

	void AssetDatabase::RecursiveCacheAssets(const std::filesystem::path& filepath)
	{
		if (!FileUtility::Exists(filepath))
		{
			MH_CORE_WARN("Could not import assets. Does the import folder exist?");
			return;
		}

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

					auto iter = s_AssetPaths.find(id);
					if (iter != s_AssetPaths.end())
						MH_CORE_WARN("Attempting to load multiple Assets with ID {0} at {1} and {2}", id, iter->second.string(), filepathUnix);

					s_AssetPaths[id] = filepathUnix;
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