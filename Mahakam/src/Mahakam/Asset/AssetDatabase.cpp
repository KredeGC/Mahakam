#include "Mahakam/mhpch.h"
#include "AssetDatabase.h"

#include "SoundAssetImporter.h"
#include "TextureAssetImporter.h"

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Random.h"

#include "Mahakam/Asset/AssetSerializeTraits.h"

#include "Mahakam/Asset/AnimationSerialization.h"
#include "Mahakam/Asset/MaterialSerialization.h"
#include "Mahakam/Asset/MeshSerialization.h"
#include "Mahakam/Asset/ShaderSerialization.h"
#include "Mahakam/Asset/TextureCubeSerialization.h"

#include <ryml/rapidyaml-0.4.1.hpp>

#include <algorithm>
#include <charconv>
#include <fstream>

namespace Mahakam
{
	template<typename Stream>
	static bool SerializeAssetHeader(Stream& stream, Mahakam::Serialization::inout<Stream, AssetID> assetID, Mahakam::Serialization::inout<Stream, std::string> extension)
	{
		return stream.serialize(assetID)
			&& stream.serialize(extension);
	}

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

	template<typename T, bool Async = false>
	static AssetDatabase::AssetSerializer CreateSerializer()
	{
		AssetDatabase::AssetSerializer serializer;
		serializer.Serialize = [](AssetDatabase::Writer& writer, const std::filesystem::path& filepath, Asset<void> asset)
		{
			return Serialization::AssetSerializeTraits<T>::serialize(writer, asset);
		};
		serializer.Deserialize = [](AssetDatabase::Reader& reader, const std::filesystem::path& filepath) -> Asset<void>
		{
			Asset<T> asset;
			if (!Serialization::AssetSerializeTraits<T>::serialize(reader, asset))
				return nullptr;

			return asset;
		};
		if constexpr (Async)
		{
			serializer.CreateEmpty = []
			{
				return T::GetDataFunctions()->CreateControlBlock();
			};
			serializer.Load = [](AssetDatabase::Reader& reader, ControlBlock* control)
			{
				// Need to rethink this:
				// When we load an asset that depends on other assets it might fail, or at the very least need a lot of tweaking
				// Eg. when loading a material it needs a shader which is only loaded after, but is required in order to set the properties
				// One solution is to instead return an object / list of dependencies to keep track of
				// When all dependencies are fully loaded we can then assemble the final asset eg. call Material::Create(..)

				Asset<T> asset;
				if (Serialization::AssetSerializeTraits<T>::serialize(reader, asset))
				{
					control->Functions->MoveConstruct(reinterpret_cast<T*>(control + 1), asset.get());
					control->State = AssetState::Loaded;
				}
				else
				{
					control->State = AssetState::Failed;
					MH_WARN("Failed to load asset with ID: {}, currently in use {} places", control->ID, control->UseCount - 1);
				}

				// Decrement so that it can be deleted
				control->UseCount--;
			};
		}

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

		LoadLegacySerializer<soundType, soundExtension>();
		LoadLegacySerializer<tex2dType, textureExtension>();
		LoadLegacySerializer<texcubeType, textureExtension>();

		// TODO: Port all legacy importers to this
		s_Serializers.emplace(animType, CreateSerializer<Animation, true>());
		s_Serializers.emplace(matType, CreateSerializer<Material, true>());
		s_Serializers.emplace(meshType, CreateSerializer<Mesh>());
		s_Serializers.emplace(shaderType, CreateSerializer<Shader>());
		//s_Serializers.emplace(texcubeType, CreateSerializer<TextureCube>());
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
	MH_DEFINE_FUNC(AssetDatabase::ReloadAsset, void, AssetID id)
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
				loadedControl->Functions->MoveAssign(control + 1, loadedControl + 1);

				auto destroy = control->Functions->Delete;
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

	//const AssetDatabase::AssetMap& AssetDatabase::GetAssetHandles()
	MH_DEFINE_FUNC(AssetDatabase::GetAssetHandles, const AssetDatabase::AssetMap&)
	{
		return s_AssetPaths;
	};

	//uint32_t AssetDatabase::GetAssetReferences(AssetDatabase::AssetID id)
	MH_DEFINE_FUNC(AssetDatabase::GetAssetReferences, size_t, AssetID id)
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

	void AssetDatabase::ProcessAssets()
	{
		if (!s_AssetQueue.empty())
			ProcessAssetFromQueue();

		if (!s_AssetFileQueue.empty())
		{

		}
	}

	void AssetDatabase::ProcessAssetFromQueue()
	{
		ReadBlock block = s_AssetQueue.pop();
		block.Load(block.FileStream, block.Control);

		if (block.Control->UseCount == 0)
		{
			UnloadAsset(block.Control);

			auto destroy = block.Control->Functions->Delete;

			MH_ASSERT(destroy, "Asset destructor encountered invalid control block");

			destroy(block.Control);
		}
	}

	ControlBlock* AssetDatabase::SaveAsset(ControlBlock* control, AssetID id, const ExtensionType& extension)
	{
		auto iter = s_Serializers.find(extension);
		MH_ASSERT(iter != s_Serializers.end(), "Asset missing serializer");
		if (iter == s_Serializers.end())
			return control;

		std::filesystem::path filepath = FileUtility::ASSET_PATH / (std::to_string(id) + FileUtility::AssetExtension);

		Writer writer(filepath);

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

		if (!iter->second.Serialize(writer, filepath, Asset<void>(control)))
		{
			MH_WARN("Failed to save asset with ID: {0}", id);
			return control;
		}

		writer.close();

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
				loadedControl->Functions->MoveAssign(control + 1, loadedControl + 1);

				// Invalidate the old control block, but don't delete it as others may reference it
				//control->MoveData = nullptr;
				//control->DeleteData = nullptr;
				control->ID = 0;
			}

			return loadedControl;
		}

		// Add as a new asset
		s_LoadedAssets.emplace(id, control);

		return control;
	}

	ControlBlock* AssetDatabase::IncrementAsset(AssetID id)
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
	}

	void AssetDatabase::UnloadAsset(ControlBlock* control)
	{
		MH_ASSERT(control->UseCount == 0, "Attempting to unload multiple instances of Asset");

		auto iter = s_LoadedAssets.find(control->ID);
		if (iter != s_LoadedAssets.end())
		{
			//if (iter->second->State == AssetState::Loaded || iter->second->State == AssetState::Streaming)
				s_LoadedAssets.erase(control->ID);
		}
		else
		{
			MH_ERROR("Attempting to unload an already unloaded asset ({0})", control->ID);
		}
	}

	ControlBlock* AssetDatabase::LoadAndIncrementAsset(AssetID id)
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

		Reader reader(filepath);

		AssetID assetID;
		std::string extension;
		if (!SerializeAssetHeader(reader, assetID, extension))
			return nullptr;

		if (assetID != id)
			MH_WARN("Asset IDs do not match. Attempted to load ID {0} but got {1}", id, assetID);

		auto iter = s_Serializers.find(extension);
		if (iter == s_Serializers.end())
			return nullptr;

		ControlBlock* control = nullptr;
		if (iter->second.CreateEmpty)
		{
			// Create an empty asset
			control = iter->second.CreateEmpty();
			control->UseCount += 2; // Increment ref count by 2. Once for the asset itself and once for the loader

			// Add the asset to a queue to process later
			s_AssetQueue.emplace(control, std::move(reader), iter->second.Load);
		}
		else
		{
			// Convert from binary to an asset
			Asset<void> asset = iter->second.Deserialize(reader, filepath);
			if (!asset)
			{
				MH_WARN("Could not load asset with ID: {0}", id);
				return nullptr;
			}

			control = asset.m_Control;
			control->UseCount++;
		}

		control->ID = id;

		s_LoadedAssets.insert({ id, control });

		return control;
	}

	void AssetDatabase::RecursiveCacheAssets(const std::filesystem::path& filepath)
	{
		if (!FileUtility::Exists(filepath))
		{
			MH_WARN("Could not import assets. Does the '{0}' folder exist?", filepath.string());
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