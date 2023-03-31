#include "Mahakam/mhpch.h"
#include "AssetDatabase.h"

#include "AssetImporter.h"
#include "AnimationAssetImporter.h"
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
		if (s_AssetImporters.find(assetImporter->GetImporterProps().Extension) == s_AssetImporters.end())
			s_AssetImporters.insert({ assetImporter->GetImporterProps().Extension, assetImporter });

		s_AssetExtensions.insert({ extension, assetImporter });
	};

	//void AssetDatabase::DeregisterAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAssetImporter, void, const std::string& extension)
	{
		auto iter = s_AssetExtensions.find(extension);
		if (iter != s_AssetExtensions.end())
		{
			long useCount = iter->second.use_count();

			ExtensionType ext = iter->second->GetImporterProps().Extension;

			s_AssetExtensions.erase(iter);

			// 2 references should exist before the erase, 1 in s_AssetExtensions and 1 in s_AssetImporters
			if (useCount <= 2)
				s_AssetImporters.erase(ext);
		}
	};

	//void AssetDatabase::DeregisterAllAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAllAssetImporters, void)
	{
		s_AssetImporters.clear();
		s_AssetExtensions.clear();
	};

	//Ref<AssetImporter> AssetDatabase::GetAssetImporter(const Extension& extension)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporter, Ref<AssetImporter>, const ExtensionType& extension)
	{
		auto iter = s_AssetImporters.find(extension);
		if (iter != s_AssetImporters.end())
			return iter->second;

		return nullptr;
	};

	//const AssetDatabase::ImporterMap& AssetDatabase::GetAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporters, const AssetDatabase::ImporterMap&)
	{
		return s_AssetImporters;
	};

	//AssetDatabase::ExtensionIter AssetDatabase::GetAssetImporterExtension(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImporterExtension, AssetDatabase::ExtensionIter, const std::string& extension)
	{
		return s_AssetExtensions.equal_range(extension);
	};

	//void AssetDatabase::RegisterDefaultAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::RegisterDefaultAssetImporters, void)
	{
		// Animation
		Ref<AnimationAssetImporter> animationAssetImporter = CreateRef<AnimationAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".gltf", animationAssetImporter);
		AssetDatabase::RegisterAssetImporter(".glb", animationAssetImporter);

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

	//void AssetDatabase::DeregisterDefaultAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::DeregisterDefaultAssetImporters, void)
	{
		// Animation
		AssetDatabase::DeregisterAssetImporter(".gltf");
		AssetDatabase::DeregisterAssetImporter(".glb");

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
		//auto iter = s_LoadedAssets.find(id);
		//if (iter != s_LoadedAssets.end())
		//{
		//	// Delete our previous data
		//	auto destroy = iter->second->DeleteData;
		//	destroy(iter->second->Ptr);

		//	// Load the asset
		//	ControlBlock* control = LoadAndIncrementAsset(id);

		//	if (control)
		//	{
		//		// Move the pointer and destructor to the existing control block
		//		iter->second->Ptr = control->Ptr;
		//		iter->second->DeleteData = control->DeleteData;

		//		// Delete the control block
		//		Allocator::Deallocate<ControlBlock>(control, 1);
		//	}
		//}
	};

	//void AssetDatabase::ReloadAssets()
	MH_DEFINE_FUNC(AssetDatabase::ReloadAssets, void)
	{
		// Recreate asset ID to filepath mapping
		RefreshAssetImports();

		// Reimport all imported assets
		for (auto& [id, loadedControl] : s_LoadedAssets)
		{
			// Load the asset
			ControlBlock* control = LoadAndIncrementAsset(id);

			if (control)
			{
				loadedControl->MoveData(control + 1, loadedControl + 1);

				auto destroy = control->DeleteData;
				destroy(control);
			}
			else
			{
				MH_WARN("Could not reload previously loaded Asset with ID: {0}", id);
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
			MH_WARN("AssetDatabase::ReadAssetInfo: The path '{0}' doesn't point to an asset", importPath.string());
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

			std::string extension;
			if (root.has_child("Extension"))
				root["Extension"] >> extension;

#ifdef MH_STANDALONE
			info.Extension = std::hash<std::string>()(extension);
#else
			info.Extension = extension;
#endif

			return info;
		}
		catch (std::runtime_error const& e)
		{
			MH_WARN("Weird yaml file found in {0}: {1}", importPath.string(), e.what());
		}

		return {};
	};

	//AssetDatabase::ControlBlock* AssetDatabase::SaveAsset(ControlBlock* control, const Extension& extension, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	MH_DEFINE_FUNC(AssetDatabase::SaveAsset, AssetDatabase::ControlBlock*, ControlBlock* control, const ExtensionType& extension, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		// Read asset info and ID
		AssetID id = 0;
		if (std::filesystem::exists(importPath))
			id = ReadAssetInfo(importPath).ID;
		else
			id = Random::GetRandomID64();

		// If no importer exists with this extension
		auto iter = s_AssetImporters.find(extension);
		if (iter == s_AssetImporters.end())
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

		iter->second->Serialize(root, control + 1);

		std::ofstream filestream(importPath);
		filestream << tree;

		filestream.close();

		auto controlIter = s_LoadedAssets.find(id);
		if (controlIter != s_LoadedAssets.end())
		{
			ControlBlock* loadedControl = controlIter->second;

			// If the control block is different, then copy and invalidate
			if (loadedControl != control)
			{
				// Increment the UseCount
				loadedControl->UseCount++;

				// Move the pointer and destructor to the existing control block
				loadedControl->MoveData(control + 1, loadedControl + 1);

				// Invalidate the old control block, but don't delete it as others may reference it
				control->MoveData = nullptr;
				//control->DeleteData = nullptr;
				control->ID = 0;
			}

			return loadedControl;
		}
		else
		{
			control->ID = id;

			s_LoadedAssets.insert({ id, control });

			RefreshAssetImports();

			return control;
		}
	};

	//AssetDatabase::ControlBlock* AssetDatabase::IncrementAsset(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::IncrementAsset, AssetDatabase::ControlBlock*, AssetDatabase::AssetID id)
	{
		MH_ASSERT(id, "Attempting to load an Asset with id 0");

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
				s_LoadedAssets.insert({ id, control });

			return control;
		}
	};

	//void AssetDatabase::DecrementAsset(ControlBlock* control)
	MH_DEFINE_FUNC(AssetDatabase::UnloadAsset, void, ControlBlock* control)
	{
		MH_ASSERT(control->UseCount == 0, "Attempting to unload multiple instances of Asset");

		if (s_LoadedAssets.find(control->ID) != s_LoadedAssets.end())
			s_LoadedAssets.erase(control->ID);
		else
			MH_WARN("Attempting to unload an already unloaded asset ({0})", control->ID);
	};

	AssetDatabase::ControlBlock* AssetDatabase::LoadAndIncrementAsset(AssetID id)
	{
		MH_ASSERT(id, "Asset ID to be loaded cannot be 0");

		// Find the import path from the ID
		std::filesystem::path importPath;
		auto iter = s_AssetPaths.find(id);
		if (iter != s_AssetPaths.end())
			importPath = iter->second;

		if (!std::filesystem::exists(importPath))
		{
			MH_WARN("AssetDatabase::LoadAssetFromID: The path '{0}' doesn't exist", importPath.string());
			return nullptr;
		}

		TrivialVector<char> buffer;
		if (!FileUtility::ReadFile(importPath, buffer))
			return nullptr;

		try
		{
			ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(buffer.data(), buffer.size()));

			ryml::NodeRef root = tree.rootref();

			// If the asset type has an importer
			std::string extension;
			if (root.has_child("Extension"))
				root["Extension"] >> extension;

#ifdef MH_STANDALONE
			auto importIter = s_AssetImporters.find(std::hash<std::string>()(extension));
#else
			auto importIter = s_AssetImporters.find(extension);
#endif
			if (importIter == s_AssetImporters.end())
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
			MH_WARN("AssetDatabase encountered exception trying to import yaml file {0}: {1}", importPath.string(), e.what());
		}

		return nullptr;
	}

	void AssetDatabase::RecursiveCacheAssets(const std::filesystem::path& filepath)
	{
		if (!FileUtility::Exists(filepath))
		{
			MH_WARN("Could not import assets. Does the import folder exist?");
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
						MH_WARN("Attempting to load multiple Assets with ID {0} at {1} and {2}", id, iter->second.string(), filepathUnix);

					s_AssetPaths[id] = filepathUnix;
				}
			}
		}
	}
}