#include "Mahakam/mhpch.h"
#include "AssetDatabase.h"

#include "AssetImporter.h"
#include "AnimationAssetImporter.h"
#include "BoneMeshAssetImporter.h"
#include "CubeMeshAssetImporter.h"
#include "CubeSphereMeshAssetImporter.h"
#include "MaterialAssetImporter.h"
#include "PlaneMeshAssetImporter.h"
#include "ShaderAssetImporter.h"
#include "SoundAssetImporter.h"
#include "TextureAssetImporter.h"
#include "UVSphereMeshAssetImporter.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Random.h"

#include "Mahakam/Serialization/YAMLGuard.h"
#include "Mahakam/Serialization/YAMLSerialization.h"

#include <ryml/rapidyaml-0.4.1.hpp>

#include <algorithm>
#include <fstream>

namespace Mahakam
{
	template<const char* Extension, const char* LegacyExt>
	void AssetDatabase::LoadLegacySerializer()
	{
		AssetSerializer serializer;
		serializer.Serialize = [](Writer& writer, const std::filesystem::path& filepath, void* asset)
			{
				// If no importer exists with this extension
				auto iter = s_AssetImporters.find(LegacyExt);
				if (iter == s_AssetImporters.end())
					return false;

				// Serialize the asset
				ryml::Tree tree;

				ryml::NodeRef root = tree.rootref();
				root |= ryml::MAP;

				iter->second->Serialize(root, asset);

				std::ofstream filestream(filepath.string() + ".legacy");
				filestream << tree;
				filestream.close();

				return true;
			};
		serializer.Deserialize = [](Reader& reader, const std::filesystem::path& filepath) -> Asset<void>
			{
				TrivialVector<char> buffer;
				if (!FileUtility::ReadFile(filepath.string() + ".legacy", buffer))
					return nullptr;

				try
				{
					ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(buffer.data(), buffer.size()));

					ryml::NodeRef root = tree.rootref();

					// If the asset type has an importer
					auto importIter = s_AssetImporters.find(LegacyExt);
					if (importIter == s_AssetImporters.end())
						return nullptr;

					// Deserialize the asset using the YAML node
					return importIter->second->Deserialize(root);
				}
				catch (std::runtime_error const& e)
				{
					MH_WARN("AssetDatabase encountered exception trying to import yaml file {0}: {1}", filepath, e.what());
				}

				return nullptr;
			};

		s_Serializers.emplace(Extension, serializer);
	}

	void AssetDatabase::LoadDefaultSerializers()
	{
		static const char animExtension[] = ".anim";
		static const char materialExtension[] = ".material";
		static const char boneExtension[] = ".bone";
		static const char cubeExtension[] = ".cube";
		static const char cubesphereExtension[] = ".cubesphere";
		static const char planeExtension[] = ".plane";
		static const char uvsphereExtension[] = ".uvsphere";
		static const char shaderExtension[] = ".shader";
		static const char soundExtension[] = ".sound";
		static const char tex2dExtension[] = ".tex2d";
		static const char texcubeExtension[] = ".texcube";
		static const char textureExtension[] = ".texture";

		LoadLegacySerializer<animExtension, animExtension>();
		LoadLegacySerializer<materialExtension, materialExtension>();
		LoadLegacySerializer<boneExtension, boneExtension>();
		LoadLegacySerializer<cubeExtension, cubeExtension>();
		LoadLegacySerializer<cubesphereExtension, cubesphereExtension>();
		LoadLegacySerializer<planeExtension, planeExtension>();
		LoadLegacySerializer<uvsphereExtension, uvsphereExtension>();
		LoadLegacySerializer<shaderExtension, shaderExtension>();
		LoadLegacySerializer<soundExtension, soundExtension>();
		LoadLegacySerializer<tex2dExtension, textureExtension>();
		LoadLegacySerializer<texcubeExtension, textureExtension>();
	}

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
		LoadDefaultSerializers();

		// Animation
		Ref<AnimationAssetImporter> animationAssetImporter = CreateRef<AnimationAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".gltf", animationAssetImporter);
		AssetDatabase::RegisterAssetImporter(".glb", animationAssetImporter);

		// Material
		Ref<MaterialAssetImporter> materialAssetImporter = CreateRef<MaterialAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".material", materialAssetImporter);

		// BoneMesh
		Ref<BoneMeshAssetImporter> boneAssetImporter = CreateRef<BoneMeshAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".gltf", boneAssetImporter);
		AssetDatabase::RegisterAssetImporter(".glb", boneAssetImporter);

		// CubeMesh
		Ref<CubeMeshAssetImporter> cubeAssetImporter = CreateRef<CubeMeshAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".cube", cubeAssetImporter);

		// CubeSphereMesh
		Ref<CubeSphereMeshAssetImporter> cubeSphereAssetImporter = CreateRef<CubeSphereMeshAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".cubesphere", cubeSphereAssetImporter);

		// PlaneMesh
		Ref<PlaneMeshAssetImporter> planeAssetImporter = CreateRef<PlaneMeshAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".plane", planeAssetImporter);

		// UVSphereMesh
		Ref<UVSphereMeshAssetImporter> uvSphereAssetImporter = CreateRef<UVSphereMeshAssetImporter>();

		AssetDatabase::RegisterAssetImporter(".uvsphere", uvSphereAssetImporter);

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

		// CubeMesh
		AssetDatabase::DeregisterAssetImporter(".cube");

		// CubeSphereMesh
		AssetDatabase::DeregisterAssetImporter(".cubesphere");

		// BoneMesh
		AssetDatabase::DeregisterAssetImporter(".gltf");
		AssetDatabase::DeregisterAssetImporter(".glb");

		// PlaneMesh
		AssetDatabase::DeregisterAssetImporter(".plane");

		// UVSphereMesh
		AssetDatabase::DeregisterAssetImporter(".uvsphere");

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
			RefreshAssetPaths();

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
		RefreshAssetPaths();

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
				MH_WARN("Could not reload previously loaded Asset with ID: {0} as it was empty", id);
			}
		}
	};

	//void AssetDatabase::RefreshAssetPaths()
	MH_DEFINE_FUNC(AssetDatabase::RefreshAssetPaths, void)
	{
		s_AssetPaths.clear();
		RecursiveCacheAssets(FileUtility::ASSET_PATH);
	};

	//const std::filesystem::path& AssetDatabase::GetAssetImportPath(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetImportPath, const std::filesystem::path&, AssetDatabase::AssetID id)
	{
		auto iter = s_AssetPaths.find(id);
		if (iter != s_AssetPaths.end())
			return iter->second;

		return EmptyPath;
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

		Reader reader(buffer.data(), static_cast<uint32_t>(buffer.size() * 8U));

		AssetInfo assetInfo;
		if (!SerializeAssetHeader(reader, assetInfo.ID, assetInfo.Extension))
			return {};

		return assetInfo;
	};

	//AssetDatabase::ControlBlock* AssetDatabase::SaveAsset(ControlBlock* control, const Extension& extension, const std::filesystem::path& filepath)
	MH_DEFINE_FUNC(AssetDatabase::SaveAsset, AssetDatabase::ControlBlock*, ControlBlock* control, AssetID id, const ExtensionType& extension, const std::filesystem::path& filepath)
	{
		auto iter = s_Serializers.find(extension);
		if (iter == s_Serializers.end())
			return nullptr;

		TrivialVector<uint32_t> buffer;
		Writer writer(buffer);

		if (control->ID != id)
			control->ID = id;

		if (!SerializeAssetHeader(writer, control->ID, extension))
			return nullptr;


		// TODO: Do this properly
		if (!iter->second.Serialize(writer, filepath, control + 1))
			return nullptr;

		uint32_t num_bits = writer.flush();


		// Create the asset directory, if it doesn't exist
		FileUtility::CreateDirectories(filepath.parent_path());

		// Save the asset
		std::ofstream filestream(filepath, std::ios::binary);
		filestream.write(reinterpret_cast<char*>(writer.get_buffer()), writer.get_num_bytes_serialized());
		filestream.close();

		// If an asset with the given ID already exists, we may need to reload it
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

		// Add as a new asset
		s_LoadedAssets.insert({ id, control });

		RefreshAssetPaths();

		return control;
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
			MH_ERROR("Attempting to unload an already unloaded asset ({0})", control->ID);
	};

	AssetDatabase::ControlBlock* AssetDatabase::LoadAndIncrementAsset(AssetID id)
	{
		MH_ASSERT(id, "Attempting to load an Asset with id 0");

		// Get asset path from ID
		auto pathIter = s_AssetPaths.find(id);
		if (pathIter == s_AssetPaths.end())
			return nullptr;

		std::filesystem::path filepath = pathIter->second;

		if (!std::filesystem::exists(filepath))
		{
			MH_WARN("AssetDatabase::ReadAsset: The path '{0}' doesn't exist", filepath.string());
			return nullptr;
		}

		// Read file into buffer
		TrivialVector<char> buffer;

		if (!FileUtility::ReadFile(filepath, buffer))
			return nullptr;

		Reader reader(buffer.data(), static_cast<uint32_t>(buffer.size() * 8U));

		AssetID assetID;
		std::string extension;
		if (!SerializeAssetHeader(reader, assetID, extension))
			return nullptr;

		auto iter = s_Serializers.find(extension);
		if (iter == s_Serializers.end())
			return nullptr;

		Asset<void> asset = iter->second.Deserialize(reader, filepath);

		asset.m_Control->ID = assetID;
		asset.IncrementRef();

		s_LoadedAssets.insert({ id, asset.m_Control });

		return asset.m_Control;
	}

	void AssetDatabase::RecursiveCacheAssets(const std::filesystem::path& filepath)
	{
		if (!FileUtility::Exists(filepath))
		{
			MH_WARN("Could not import assets. Does the '{0}' folder exist?", filepath.u8string());
			return;
		}

		auto iter = std::filesystem::directory_iterator(filepath);

		for (auto& directory : iter)
		{
			if (directory.is_directory())
			{
				RecursiveCacheAssets(directory.path());
			}
			else if (directory.path().extension() == FileUtility::AssetExtension)
			{
				// IDEA: Make assets have their ID as filename
				// That way there's no need to read in order to index
				TrivialVector<char> buffer;

				if (!FileUtility::ReadFile(directory.path(), buffer))
					continue;

				bitstream::fixed_bit_reader reader(buffer.data(), static_cast<uint32_t>(buffer.size() * 8U));

				AssetID id;
				std::string extension;
				if (!SerializeAssetHeader(reader, id, extension))
					continue;

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
			else
			{
				MH_WARN("Found a file without the .asset extension inside the assets/ directory: {0}", directory.path().string());
			}
		}
	}
}