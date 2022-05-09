#pragma once
#include "Core.h"
#include "Log.h"
#include "Utility.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string>

namespace Mahakam
{
	class AssetImporter;

	class AssetDatabase
	{
	public:
		struct AssetInfo
		{
			uint64_t ID;
			std::filesystem::path Filepath;
			std::string Extension;
		};

	private:
		using AssetMap = std::unordered_map<uint64_t, std::filesystem::path>;

		inline static std::unordered_map<std::string, Ref<AssetImporter>> m_AssetImporters;

		inline static AssetMap m_AssetPaths;

		inline static std::unordered_map<uint64_t, WeakRef<void>> m_CachedAssets;

		inline static std::unordered_map<void*, uint64_t> m_AssetIndex;

	public:
		// Registering asset importers
		MH_DECLARE_FUNC(RegisterAssetImporter, void, const std::string& extension, Ref<AssetImporter> assetImport);
		MH_DECLARE_FUNC(DeregisterAssetImporter, void, const std::string& extension);
		MH_DECLARE_FUNC(GetAssetImporter, Ref<AssetImporter>, const std::string& extension);

		// Miscellaneous functions
		MH_DECLARE_FUNC(CollectGarbage, void);
		MH_DECLARE_FUNC(ReloadAssetImports, void);
		MH_DECLARE_FUNC(GetAssetHandles, AssetMap);
		MH_DECLARE_FUNC(GetAssetReferences, uint32_t, uint64_t id);
		MH_DECLARE_FUNC(GetAssetID, uint64_t, Ref<void> asset);
		MH_DECLARE_FUNC(GetAssetInfo, AssetInfo, const std::filesystem::path& importPath);

		// Saving and loading assets
		MH_DECLARE_FUNC(SaveAsset, void, Ref<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath);
		inline static Ref<void> LoadAsset(const std::filesystem::path& importPath) { return LoadAssetFromPath(importPath); }
		inline static Ref<void> LoadAsset(uint64_t id) { return LoadAssetFromID(id); }

		// Template functions for convenience
		template<typename T>
		static Ref<T> LoadAsset(const std::filesystem::path& importPath)
		{
			return StaticCastRef<T>(AssetDatabase::LoadAsset(importPath));
		}

		template<typename T>
		static Ref<T> LoadAsset(uint64_t id)
		{
			return StaticCastRef<T>(AssetDatabase::LoadAsset(id));
		}

	private:
		MH_DECLARE_FUNC(LoadAssetFromPath, Ref<void>, const std::filesystem::path& importPath);
		MH_DECLARE_FUNC(LoadAssetFromID, Ref<void>, uint64_t id);

		static uint64_t ReadAssetID(const std::filesystem::path& filepath);

		static void RecursiveCacheAssets(const std::filesystem::path& filepath);

		static void RecursiveImportAssets(const std::filesystem::path& filepath);



		//static void* SerializeMesh(Ref<Mesh> src, uint32_t& size)
		//{
		//	const uint32_t vertexCount = src->GetVertexCount();
		//	const uint32_t indexCount = src->GetIndexCount();

		//	// Determine size
		//	size = 7 * sizeof(uint32_t); // Add the sizeof uint32 as amount
		//	size += src->HasVertices(0) ? vertexCount * sizeof(glm::vec3) : 0;
		//	size += src->HasVertices(1) ? vertexCount * sizeof(glm::vec2) : 0;
		//	size += src->HasVertices(2) ? vertexCount * sizeof(glm::vec3) : 0;
		//	size += src->HasVertices(3) ? vertexCount * sizeof(glm::vec3) : 0;
		//	size += src->HasVertices(4) ? vertexCount * sizeof(glm::vec4) : 0;
		//	size += src->HasVertices(5) ? vertexCount * sizeof(glm::ivec4) : 0;
		//	size += src->HasVertices(6) ? vertexCount * sizeof(glm::vec4) : 0;

		//	size += src->GetIndexCount() * sizeof(uint32_t);

		//	size += 2 * sizeof(glm::vec3);

		//	// Populate with vertex data
		//	char* data = new char[size];

		//	uint32_t offset = 0;

		//	for (uint32_t i = 0; i < Mesh::BUFFER_ELEMENTS_SIZE; i++)
		//	{
		//		if (!src->HasVertices(i))
		//		{
		//			const uint32_t elementSize = 0;
		//			memcpy(data + offset, &elementSize, sizeof(uint32_t));
		//			offset += sizeof(uint32_t);
		//		}
		//		else
		//		{

		//			uint32_t elementSize = vertexCount;
		//			switch (i)
		//			{
		//			case 0:
		//			case 2:
		//			case 3:
		//				elementSize *= sizeof(glm::vec3);
		//				break;
		//			case 1:
		//				elementSize *= sizeof(glm::vec2);
		//				break;
		//			case 4:
		//			case 6:
		//				elementSize *= sizeof(glm::vec4);
		//				break;
		//			case 5:
		//				elementSize *= sizeof(glm::ivec4);
		//				break;
		//			}

		//			memcpy(data + offset, &elementSize, sizeof(uint32_t));
		//			memcpy(data + offset + sizeof(uint32_t), src->GetVertices(i), elementSize);
		//			offset += elementSize + sizeof(uint32_t);
		//		}
		//	}

		//	// Populate index data
		//	memcpy(data + offset, src->GetIndices(), indexCount * sizeof(uint32_t));
		//	offset += indexCount * sizeof(uint32_t);

		//	// Populate bounds
		//	memcpy(data + offset, &src->GetBounds(), 2 * sizeof(glm::vec3));

		//	return data;
		//}

		//static Ref<Mesh> DeserializeMesh(char* data, uint32_t size)
		//{
		//	// TODO: Fix the rest of this mess
		//	Mesh::InterleavedStruct interleavedVertices;

		//	uint32_t offset = 0;

		//	uint32_t posSize = 0;
		//	memcpy(&posSize, data + offset, sizeof(uint32_t));
		//	offset += sizeof(uint32_t);
		//	if (posSize > 0)
		//	{
		//		posSize /= sizeof(glm::vec3);
		//		interleavedVertices.positions = new glm::vec3[posSize];
		//		memcpy(interleavedVertices.positions, data + offset, posSize);
		//		offset += posSize;
		//	}

		//	return nullptr;
		//}

	public:
		//// Texture2D
		//template<typename T, typename = typename std::enable_if<std::is_same<T, Texture2D>::value, void>::type>
		//static auto CreateOrLoadAsset(const std::string& src, bool saveMips, const TextureProps& props, typename std::enable_if<std::is_same<T, Texture2D>::value, void>::type* dummy = nullptr)
		//{
		//	MH_CORE_ASSERT(props.width && props.height, "Width and height must be specified when loading or saving assets!");

		//	const std::string cachepath = FileUtility::GetResourcePath(src);

		//	if (!FileUtility::Exists(cachepath))
		//	{
		//		Ref<T> asset = T::Create(src, props);

		//		uint32_t size = asset->GetSize();
		//		uint32_t totalSize = asset->GetTotalSize();

		//		// Save to cache
		//		char* pixels = new char[totalSize];
		//		asset->ReadPixels(pixels, saveMips);
		//		std::ofstream stream(cachepath, std::ios::binary);
		//		stream.write((char*)&size, sizeof(uint32_t));
		//		stream.write(pixels, totalSize);

		//		delete[] pixels;

		//		return asset;
		//	}
		//	else
		//	{
		//		// Load from cache
		//		std::ifstream inStream(cachepath, std::ios::binary);
		//		std::stringstream ss;
		//		uint32_t size = 0;
		//		inStream.read((char*)&size, sizeof(uint32_t));
		//		ss << inStream.rdbuf();
		//		Ref<T> lut = T::Create(props);
		//		lut->SetData((void*)ss.str().c_str(), size, saveMips);

		//		return lut;
		//	}
		//}

		//// TextureCube
		//template<typename T, typename = typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type>
		//static auto CreateOrLoadAsset(const std::string& src, bool saveMips, const CubeTextureProps& props, typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type* dummy = nullptr)
		//{
		//	MH_CORE_ASSERT(props.resolution, "Resolution must be specified when loading or saving assets!");

		//	const std::string cachepath = FileUtility::GetResourcePath(src);

		//	if (!FileUtility::Exists(cachepath))
		//	{
		//		Ref<T> asset = T::Create(src, props);

		//		uint32_t size = asset->GetSize();
		//		uint32_t totalSize = asset->GetTotalSize();

		//		// Save to cache
		//		char* pixels = new char[totalSize];
		//		asset->readPixels(pixels, saveMips);
		//		std::ofstream stream(cachepath, std::ios::binary);
		//		stream.write((char*)&size, sizeof(uint32_t));
		//		stream.write(pixels, totalSize);

		//		delete[] pixels;

		//		return asset;
		//	}
		//	else
		//	{
		//		// Load from cache
		//		std::ifstream inStream(cachepath, std::ios::binary);
		//		std::stringstream ss;
		//		uint32_t size = 0;
		//		inStream.read((char*)&size, sizeof(uint32_t));
		//		ss << inStream.rdbuf();
		//		Ref<T> lut = T::Create(props);
		//		lut->SetData((void*)ss.str().c_str(), size, saveMips);

		//		return lut;
		//	}
		//}

		//// TextureCube - Convolute or prefilter
		//template<typename T, typename = typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type>
		//static auto CreateOrLoadAsset(const std::string& filepath, Ref<T> src, bool saveMips, TextureCubePrefilter prefilter, const CubeTextureProps& props, typename std::enable_if<std::is_same<T, TextureCube>::value, void>::type* dummy = nullptr)
		//{
		//	MH_CORE_ASSERT(props.resolution, "Resolution must be specified when loading or saving assets!");

		//	const std::string cachepath = FileUtility::GetResourcePath(filepath);

		//	if (!FileUtility::Exists(cachepath))
		//	{
		//		Ref<T> asset = T::Create(src, prefilter, props);

		//		uint32_t size = asset->GetSize();
		//		uint32_t totalSize = asset->GetTotalSize();

		//		// Save to cache
		//		char* pixels = new char[totalSize];
		//		asset->ReadPixels(pixels, saveMips);
		//		std::ofstream stream(cachepath, std::ios::binary);
		//		stream.write((char*)&size, sizeof(uint32_t));
		//		stream.write(pixels, totalSize);

		//		delete[] pixels;

		//		return asset;
		//	}
		//	else
		//	{
		//		// Load from cache
		//		std::ifstream inStream(cachepath, std::ios::binary);
		//		std::stringstream ss;
		//		uint32_t size = 0;
		//		inStream.read((char*)&size, sizeof(uint32_t));
		//		ss << inStream.rdbuf();
		//		Ref<T> lut = T::Create(props);
		//		lut->SetData((void*)ss.str().c_str(), size, saveMips);

		//		return lut;
		//	}
		//}

		//template<typename T, typename = typename std::enable_if<std::is_same<T, SkinnedMesh>::value, void>::type>
		//static auto CreateOrLoadAsset(const std::string& src, const SkinnedMeshProps& props = SkinnedMeshProps(), typename std::enable_if<std::is_same<T, SkinnedMesh>::value, void>::type* dummy = nullptr)
		//{
		//	const std::string cachepath = FileUtility::GetResourcePath(src);

		//	if (!FileUtility::Exists(cachepath))
		//	{
		//		auto skinnedMesh = Mesh::LoadModel(src, props);

		//		std::ofstream stream(cachepath, std::ios::binary);

		//		uint32_t meshCount = (uint32_t)skinnedMesh.meshes.size();
		//		uint32_t boneCount = (uint32_t)skinnedMesh.boneCount;

		//		stream.write((char*)&meshCount, sizeof(uint32_t));
		//		for (uint32_t i = 0; i < meshCount; i++)
		//		{
		//			uint32_t meshDataSize = 0;
		//			void* meshData = SerializeMesh(skinnedMesh.meshes[i], meshDataSize);

		//			stream.write((char*)&meshDataSize, sizeof(uint32_t));
		//			stream.write((const char*)meshData, meshDataSize);

		//			delete[] meshData;
		//		}

		//		return skinnedMesh;
		//	}
		//	else
		//	{
		//		SkinnedMesh skinnedMesh;

		//		std::ifstream stream(cachepath, std::ios::binary);

		//		uint32_t meshCount = 0;
		//		uint32_t boneCount = 0;

		//		stream.read((char*)&meshCount, sizeof(uint32_t));
		//		for (uint32_t i = 0; i < meshCount; i++)
		//		{
		//			uint32_t meshDataSize = 0;
		//			stream.read((char*)&meshDataSize, sizeof(uint32_t));

		//			char* meshData = new char[meshDataSize];
		//			stream.read(meshData, meshDataSize);

		//			Ref<Mesh> mesh = DeserializeMesh(meshData, meshDataSize);

		//			delete[] meshData;
		//		}

		//		return skinnedMesh;
		//	}
		//}
	};
}