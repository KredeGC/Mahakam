#pragma once

#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/Texture.h"

#include "Mahakam/BinarySerialization/AssetSerialization.h"
#include "Mahakam/BinarySerialization/ContainerSerialization.h"
#include "Mahakam/BinarySerialization/MeshSerialization.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <bitstream.h>
#include <ryml/rapidyaml-0.4.1.hpp>

#include <cstdint>

namespace Mahakam
{
	// TODO: Why not use serialize_traits directly?
	template<typename>
	struct AssetSerializeTraits;

	template<>
	struct AssetSerializeTraits<TextureCube>
	{
		static bool Serialize(AssetDatabase::Writer& writer, const std::filesystem::path& filepath, void* asset)
		{
			TextureCube* textureAsset = static_cast<TextureCube*>(asset);

			/*writer.serialize<TextureFormat>(textureAsset->GetProps().Format);
			writer.serialize<TextureFilter>(textureAsset->GetProps().FilterMode);
			writer.serialize<uint32_t>(textureAsset->GetProps().Resolution);
			writer.serialize<TextureCubePrefilter>(textureAsset->GetProps().Prefilter);
			writer.serialize<bool>(textureAsset->GetProps().Mipmaps);*/

			void* pixels;

			textureAsset->ReadPixels(pixels);
		}

		static Asset<void> Deserialize(AssetDatabase::Reader& reader, const std::filesystem::path& filepath)
		{
			CubeTextureProps props;

			/*reader.serialize<TextureFormat>(props.Format);
			reader.serialize<TextureFilter>(props.FilterMode);
			reader.serialize<uint32_t>(props.Resolution);
			reader.serialize<TextureCubePrefilter>(props.Prefilter);
			reader.serialize<bool>(props.Mipmaps);*/

			return nullptr;
		}
	};

	template<>
	struct AssetSerializeTraits<Mesh>
	{
		template<typename Stream>
		typename bitstream::utility::is_writing_t<Stream>
		static Serialize(Stream& writer, Mesh* asset) noexcept
		{
			BS_ASSERT(writer.serialize(asset->GetProps()));

			// NodeHierarchy and bones
			BS_ASSERT(writer.serialize(asset->NodeHierarchy));
			BS_ASSERT(writer.serialize(asset->Skins));
			BS_ASSERT(writer.serialize(asset->SubMeshMap));
			BS_ASSERT(writer.serialize(asset->BoneMap));

			// Write submeshes
			BS_ASSERT(writer.serialize(asset->Meshes.size()));

			// Submeshes
			BS_ASSERT(writer.serialize(asset->Meshes));

			return true;
		}
		
		template<typename Stream>
		typename bitstream::utility::is_reading_t<Stream, Asset<Mesh>>
		static Deserialize(Stream& reader) noexcept
		{
			MeshProps props;
			BS_ASSERT(reader.serialize(props));

			Asset<Mesh> mesh = CreateAsset<Mesh>(props);

			// NodeHierarchy and bones
			BS_ASSERT(reader.serialize(mesh->NodeHierarchy));
			BS_ASSERT(reader.serialize(mesh->Skins));
			BS_ASSERT(reader.serialize(mesh->SubMeshMap));
			BS_ASSERT(reader.serialize(mesh->BoneMap));

			// Submeshes
			BS_ASSERT(reader.serialize(mesh->Meshes));

			return mesh;
		}
	};
}