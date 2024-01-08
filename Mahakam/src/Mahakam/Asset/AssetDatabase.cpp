#include "Mahakam/mhpch.h"
#include "AssetDatabase.h"

#include "AssetImporter.h"
#include "AnimationAssetImporter.h"
#include "MaterialAssetImporter.h"
#include "ShaderAssetImporter.h"
#include "SoundAssetImporter.h"
#include "TextureAssetImporter.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Random.h"

#include "Mahakam/BinarySerialization/AssetSerialization.h"
#include "Mahakam/BinarySerialization/MeshSerialization.h"

// TEMP
#include "Mahakam/Serialization/YAMLGuard.h"
#include "Mahakam/Serialization/YAMLSerialization.h"
// TEMP

#include <ryml/rapidyaml-0.4.1.hpp>

#include <algorithm>
#include <charconv>
#include <fstream>

namespace Mahakam
{
	template<const char* Extension, const char* LegacyExt>
	void AssetDatabase::LoadLegacySerializer()
	{
		AssetSerializer serializer;
		serializer.Serialize = [](Writer& writer, const std::filesystem::path& filepath, Asset<void> asset)
			{
				// If no importer exists with this extension
				auto iter = s_AssetImporters.find(LegacyExt);
				if (iter == s_AssetImporters.end())
					return false;

				// Serialize the asset
				ryml::Tree tree;

				ryml::NodeRef root = tree.rootref();
				root |= ryml::MAP;

				iter->second->Serialize(root, asset.get());

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

	template<typename T>
	AssetDatabase::AssetSerializer CreateSerializer()
	{
		AssetDatabase::AssetSerializer serializer;
		serializer.Serialize = [](AssetDatabase::Writer& writer, const std::filesystem::path& filepath, Asset<void> asset)
		{
			return writer.serialize<T>(asset);

			//return AssetSerializeTraits<T>::Serialize(writer, static_cast<T*>(asset));
		};
		serializer.Deserialize = [](AssetDatabase::Reader& reader, const std::filesystem::path& filepath) -> Asset<void>
		{
			Asset<T> asset;
			if (!reader.serialize<T>(asset))
				return nullptr;

			return asset;

			//return AssetSerializeTraits<T>::Deserialize(reader);
		};

		return serializer;
	}

	void AssetDatabase::LoadDefaultSerializers()
	{
		static const char animExtension[] = ".anim";
		static const char materialExtension[] = ".material";
		static const char shaderExtension[] = ".shader";
		static const char soundExtension[] = ".sound";
		static const char tex2dExtension[] = ".tex2d";
		static const char texcubeExtension[] = ".texcube";
		static const char textureExtension[] = ".texture";

		static const char animType[] = "anim";
		static const char matType[] = "mat";
		static const char shaderType[] = "shader";
		static const char soundType[] = "sound";
		static const char tex2dType[] = "tex2d";
		static const char texcubeType[] = "texcube";
		static const char meshType[] = "mesh";

		LoadLegacySerializer<animType, animExtension>();
		LoadLegacySerializer<matType, materialExtension>();
		LoadLegacySerializer<shaderType, shaderExtension>();
		LoadLegacySerializer<soundType, soundExtension>();
		LoadLegacySerializer<tex2dType, textureExtension>();
		LoadLegacySerializer<texcubeType, textureExtension>();

		s_Serializers.emplace(meshType, CreateSerializer<Mesh>());
	}

	//void AssetDatabase::RegisterAssetImporter(Ref<AssetImporter> assetImport)
	MH_DEFINE_FUNC(AssetDatabase::RegisterAssetImporter, void, Ref<AssetImporter> assetImporter)
	{
		if (s_AssetImporters.find(assetImporter->GetImporterProps().Extension) == s_AssetImporters.end())
			s_AssetImporters.insert({ assetImporter->GetImporterProps().Extension, assetImporter });
	};

	//void AssetDatabase::DeregisterAssetImporter(const std::string& extension)
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAssetImporter, void, const std::string& extension)
	{
		s_AssetImporters.erase(extension);
	};

	//void AssetDatabase::DeregisterAllAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::DeregisterAllAssetImporters, void)
	{
		s_AssetImporters.clear();
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

	//void AssetDatabase::RegisterDefaultAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::RegisterDefaultAssetImporters, void)
	{
		LoadDefaultSerializers();

		// Animation
		AssetDatabase::RegisterAssetImporter(CreateRef<AnimationAssetImporter>());

		// Material
		AssetDatabase::RegisterAssetImporter(CreateRef<MaterialAssetImporter>());

		// Shader
		AssetDatabase::RegisterAssetImporter(CreateRef<ShaderAssetImporter>());

		// Sound
		AssetDatabase::RegisterAssetImporter(CreateRef<SoundAssetImporter>());

		// Texture
		AssetDatabase::RegisterAssetImporter(CreateRef<TextureAssetImporter>());
	};

	//void AssetDatabase::DeregisterDefaultAssetImporters()
	MH_DEFINE_FUNC(AssetDatabase::DeregisterDefaultAssetImporters, void)
	{
		// Animation
		AssetDatabase::DeregisterAssetImporter(".anim");

		// Material
		AssetDatabase::DeregisterAssetImporter(".material");

		// CubeMesh
		AssetDatabase::DeregisterAssetImporter(".cube");

		// CubeSphereMesh
		AssetDatabase::DeregisterAssetImporter(".cubesphere");

		// BoneMesh
		AssetDatabase::DeregisterAssetImporter(".bone");

		// PlaneMesh
		AssetDatabase::DeregisterAssetImporter(".plane");

		// UVSphereMesh
		AssetDatabase::DeregisterAssetImporter(".uvsphere");

		// Shader
		AssetDatabase::DeregisterAssetImporter(".shader");

		// Sound
		AssetDatabase::DeregisterAssetImporter(".sound");

		// Texture
		AssetDatabase::DeregisterAssetImporter(".texture");
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

	//bool AssetDatabase::AssetExists(AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::AssetExists, bool, AssetID id)
	{
		auto iter = s_AssetPaths.find(id);
		
		return iter != s_AssetPaths.end();
	};

	//AssetDatabase::ControlBlock* AssetDatabase::SaveAsset(ControlBlock* control, const Extension& extension)
	MH_DEFINE_FUNC(AssetDatabase::SaveAsset, AssetDatabase::ControlBlock*, ControlBlock* control, AssetID id, const ExtensionType& extension)
	{
		auto iter = s_Serializers.find(extension);
		MH_ASSERT(iter != s_Serializers.end(), "Asset missing serializer");
		if (iter == s_Serializers.end())
			return control;

		TrivialVector<uint32_t> buffer;
		Writer writer(buffer);

		// The asset must either be blank or use the same ID as earlier
		if (control->ID && control->ID != id)
		{
			MH_WARN("Asset IDs do not match. Attempted to override existing ID {0} with {1}", control->ID, id);
			return control;
		}

		control->ID = id;

		if (!SerializeAssetHeader(writer, control->ID, extension))
		{
			MH_WARN("Failed to serialize header of asset with ID: {0}", id);
			return control;
		}

		std::filesystem::path filepath = FileUtility::ASSET_PATH / (std::to_string(id) + FileUtility::AssetExtension);
		if (!iter->second.Serialize(writer, filepath, Asset<void>(control)))
		{
			MH_WARN("Failed to save asset with ID: {0}", id);
			return control;
		}

		writer.flush();

		// Save the asset
		std::ofstream filestream(filepath, std::ios::binary);
		filestream.write(reinterpret_cast<char*>(writer.get_buffer()), writer.get_num_bytes_serialized());
		filestream.close();

		s_AssetPaths[id] = filepath;

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
		s_LoadedAssets.emplace(id, control);

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
			MH_WARN("The path '{0}' doesn't exist", filepath.string());
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

		if (assetID != id)
			MH_WARN("Asset IDs do not match. Attempted to load ID {0} but got {1}", id, assetID);

		auto iter = s_Serializers.find(extension);
		if (iter == s_Serializers.end())
			return nullptr;

		// Convert from binary to an asset
		Asset<void> asset = iter->second.Deserialize(reader, filepath);
		if (!asset)
		{
			MH_WARN("Could not load asset with ID: {0}", id);
			return nullptr;
		}

		asset.m_Control->ID = id;
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
				std::string pathString = directory.path().filename().string();
				AssetID id;
				if (std::from_chars(pathString.data(), pathString.data() + pathString.size(), id).ec != std::errc{})
					continue;

				auto iter = s_AssetPaths.find(id);
				if (iter != s_AssetPaths.end())
					MH_WARN("Attempting to load multiple Assets with ID {0} at {1} and {2}", id, iter->second.string(), directory.path().string());

				s_AssetPaths[id] = directory.path().string();
			}
			else
			{
				MH_WARN("Found a file without the .asset extension inside the assets/ directory: {0}", directory.path().string());
			}
		}
	}
}