#include "Mahakam/mhpch.h"
#include "ResourceRegistry.h"

#include "ResourceImporter.h"

#include "AnimationResourceImporter.h"
#include "BoneMeshResourceImporter.h"
#include "CubeMeshResourceImporter.h"
#include "CubeSphereMeshResourceImporter.h"
#include "MaterialResourceImporter.h"
#include "PlaneMeshResourceImporter.h"
#include "ShaderResourceImporter.h"
#include "SoundResourceImporter.h"
#include "Texture2DResourceImporter.h"
#include "TextureCubeResourceImporter.h"
#include "UVSphereMeshResourceImporter.h"

namespace Mahakam
{
	void ResourceRegistry::RegisterAssetImporter(const std::string& extension, Ref<ResourceImporter> assetImporter)
	{
		if (s_AssetImporters.find(assetImporter->GetImporterProps().Extension) == s_AssetImporters.end())
			s_AssetImporters.insert({ assetImporter->GetImporterProps().Extension, assetImporter });

		s_AssetExtensions.insert({ extension, assetImporter });
	}

	void ResourceRegistry::DeregisterAssetImporter(const std::string& extension)
	{
		auto iter = s_AssetExtensions.find(extension);
		if (iter != s_AssetExtensions.end())
		{
			long useCount = iter->second.use_count();

			std::string ext = iter->second->GetImporterProps().Extension;

			s_AssetExtensions.erase(iter);

			// 2 references should exist before the erase, 1 in s_AssetExtensions and 1 in s_AssetImporters
			if (useCount <= 2)
				s_AssetImporters.erase(ext);
		}
	}

	void ResourceRegistry::DeregisterAllAssetImporters()
	{
		s_AssetImporters.clear();
		s_AssetExtensions.clear();
	}

	Ref<ResourceImporter> ResourceRegistry::GetAssetImporter(const std::string& extension)
	{
		auto iter = s_AssetImporters.find(extension);
		if (iter != s_AssetImporters.end())
			return iter->second;

		return nullptr;
	}

	const ResourceRegistry::ImporterMap& ResourceRegistry::GetAssetImporters()
	{
		return s_AssetImporters;
	}

	ResourceRegistry::ExtensionIter ResourceRegistry::GetAssetImporterExtension(const std::string& extension)
	{
		return s_AssetExtensions.equal_range(extension);
	}

	void ResourceRegistry::RegisterDefaultAssetImporters()
	{
		// Animation
		Ref<AnimationResourceImporter> animationImporter = CreateRef<AnimationResourceImporter>();

		RegisterAssetImporter(".gltf", animationImporter);
		RegisterAssetImporter(".glb", animationImporter);

		// Material
		Ref<MaterialResourceImporter> materialImporter = CreateRef<MaterialResourceImporter>();

		RegisterAssetImporter(".material", materialImporter);

		// BoneMesh
		Ref<BoneMeshResourceImporter> boneImporter = CreateRef<BoneMeshResourceImporter>();

		RegisterAssetImporter(".gltf", boneImporter);
		RegisterAssetImporter(".glb", boneImporter);

		// CubeMesh
		Ref<CubeMeshResourceImporter> cubeImporter = CreateRef<CubeMeshResourceImporter>();

		RegisterAssetImporter(".cube", cubeImporter);

		// CubeSphereMesh
		Ref<CubeSphereMeshResourceImporter> cubeSphereImporter = CreateRef<CubeSphereMeshResourceImporter>();

		RegisterAssetImporter(".cubesphere", cubeSphereImporter);

		// PlaneMesh
		Ref<PlaneMeshResourceImporter> planeImporter = CreateRef<PlaneMeshResourceImporter>();

		RegisterAssetImporter(".plane", planeImporter);

		// UVSphereMesh
		Ref<UVSphereMeshResourceImporter> uvSphereImporter = CreateRef<UVSphereMeshResourceImporter>();

		RegisterAssetImporter(".uvsphere", uvSphereImporter);

		// Shader
		Ref<ShaderResourceImporter> shaderImporter = CreateRef<ShaderResourceImporter>();

		RegisterAssetImporter(".shader", shaderImporter);

		// Sound
		Ref<SoundResourceImporter> soundImporter = CreateRef<SoundResourceImporter>();

		RegisterAssetImporter(".wav", soundImporter);
		RegisterAssetImporter(".mp3", soundImporter);

		// Texture2D
		Ref<Texture2DResourceImporter> texture2DImporter = CreateRef<Texture2DResourceImporter>();

		RegisterAssetImporter(".png", texture2DImporter);
		RegisterAssetImporter(".jpeg", texture2DImporter);
		RegisterAssetImporter(".jpg", texture2DImporter);
		//RegisterAssetImporter(".hdr", texture2DImporter);

		// TextureCube
		Ref<TextureCubeResourceImporter> textureCubeImporter = CreateRef<TextureCubeResourceImporter>();

		RegisterAssetImporter(".png", textureCubeImporter);
		RegisterAssetImporter(".jpeg", textureCubeImporter);
		RegisterAssetImporter(".jpg", textureCubeImporter);
		RegisterAssetImporter(".hdr", textureCubeImporter);
	}

	void ResourceRegistry::DeregisterDefaultAssetImporters()
	{
		// Animation
		DeregisterAssetImporter(".gltf");
		DeregisterAssetImporter(".glb");

		// Material
		DeregisterAssetImporter(".material");

		// CubeMesh
		DeregisterAssetImporter(".cube");

		// CubeSphereMesh
		DeregisterAssetImporter(".cubesphere");

		// BoneMesh
		DeregisterAssetImporter(".gltf");
		DeregisterAssetImporter(".glb");

		// PlaneMesh
		DeregisterAssetImporter(".plane");

		// UVSphereMesh
		DeregisterAssetImporter(".uvsphere");

		// Shader
		DeregisterAssetImporter(".shader");

		// Sound
		DeregisterAssetImporter(".wav");
		DeregisterAssetImporter(".mp3");

		// Texture2D
		DeregisterAssetImporter(".png");
		DeregisterAssetImporter(".jpeg");
		DeregisterAssetImporter(".jpg");
		//DeregisterAssetImporter(".hdr");

		// TextureCube
		DeregisterAssetImporter(".png");
		DeregisterAssetImporter(".jpeg");
		DeregisterAssetImporter(".jpg");
		DeregisterAssetImporter(".hdr");
	}

	void ResourceRegistry::RefreshImportPaths()
	{
		s_ImportPaths.clear();
		RecursiveCacheAssets(FileUtility::IMPORT_PATH);
	}

	ResourceRegistry::ImportInfo ResourceRegistry::GetImportInfo(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath) || std::filesystem::is_directory(filepath))
		{
			MH_WARN("ResourceRegistry::GetImportInfo: The path '{0}' doesn't point to an asset", filepath.string());
			return {};
		}

		TrivialVector<char> buffer;

		if (!FileUtility::ReadFile(filepath, buffer))
			return {};

		try
		{
			ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(buffer.data(), buffer.size()));

			ryml::NodeRef root = tree.rootref();

			if (!root.valid())
				return {};

			ImportInfo info;
			info.Filepath = filepath;

			if (root.has_child("ID"))
				root["ID"] >> info.ID;

			if (root.has_child("Extension"))
				root["Extension"] >> info.Type;

			return info;
		}
		catch (std::runtime_error const& e)
		{
			MH_WARN("Weird yaml file found in {0}: {1}", filepath.string(), e.what());
		}

		return {};
	}

	const ResourceRegistry::ImportInfo& ResourceRegistry::GetImportInfo(AssetID id)
	{
		auto iter = s_ImportPaths.find(id);
		if (iter == s_ImportPaths.end())
			return EmptyInfo;

		return iter->second;
	}

	void ResourceRegistry::RecursiveCacheAssets(const std::filesystem::path& filepath)
	{
		if (!FileUtility::Exists(filepath))
		{
			MH_WARN("Could not import assets. Does the '{0}' folder exist?", filepath.u8string());
			return;
		}

		auto iter = std::filesystem::directory_iterator(filepath);

		for (auto& directory : iter)
		{
			std::filesystem::path path = directory.path();

			if (directory.is_directory())
			{
				RecursiveCacheAssets(path);
			}
			else if (path.extension() == ImportExtension)
			{
				ImportInfo info = GetImportInfo(path);

				if (info.ID)
				{
					auto iter = s_ImportPaths.find(info.ID);
					if (iter != s_ImportPaths.end())
						MH_WARN("Attempting to load multiple Assets with ID {0} at {1} and {2}", info.ID, iter->second.Filepath.string(), path.string());

					s_ImportPaths[info.ID] = info;
				}
			}
			else
			{
				MH_WARN("Found a file without the .import extension at: {0}", path.string());
			}
		}
	}
}