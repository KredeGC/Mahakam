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

		// Texture
		Ref<Texture2DResourceImporter> texture2DImporter = CreateRef<Texture2DResourceImporter>();

		RegisterAssetImporter(".png", texture2DImporter);
		RegisterAssetImporter(".jpeg", texture2DImporter);
		RegisterAssetImporter(".jpg", texture2DImporter);
		//RegisterAssetImporter(".hdr", texture2DImporter);
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

		// Texture
		DeregisterAssetImporter(".png");
		DeregisterAssetImporter(".jpeg");
		DeregisterAssetImporter(".jpg");
		DeregisterAssetImporter(".hdr");
	}
}