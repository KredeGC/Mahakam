#pragma once

#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/Texture.h"

#include "Mahakam/BinarySerialization/AssetSerialization.h"

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

			writer.serialize<TextureFormat>(textureAsset->GetProps().Format);
			writer.serialize<TextureFilter>(textureAsset->GetProps().FilterMode);
			writer.serialize<uint32_t>(textureAsset->GetProps().Resolution);
			writer.serialize<TextureCubePrefilter>(textureAsset->GetProps().Prefilter);
			writer.serialize<bool>(textureAsset->GetProps().Mipmaps);

			void* pixels;

			textureAsset->ReadPixels(pixels);
		}

		static Asset<void> Deserialize(AssetDatabase::Reader& reader, const std::filesystem::path& filepath)
		{
			CubeTextureProps props;

			reader.serialize<TextureFormat>(props.Format);
			reader.serialize<TextureFilter>(props.FilterMode);
			reader.serialize<uint32_t>(props.Resolution);
			reader.serialize<TextureCubePrefilter>(props.Prefilter);
			reader.serialize<bool>(props.Mipmaps);

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
			// Write materials
			BS_ASSERT(writer.template serialize<size_t>(asset->GetProps().Materials.size()));

			for (auto& material : asset->GetProps().Materials)
				BS_ASSERT(writer.template serialize<Asset<Material>>(material));

			// IncludeNodes and IncludeBones
			BS_ASSERT(writer.template serialize<bool>(asset->GetProps().IncludeBones));
			BS_ASSERT(writer.template serialize<bool>(asset->GetProps().IncludeNodes));

			// TODO: Bones, nodes, hierarchy etc.


			// Write submeshes
			BS_ASSERT(writer.template serialize<size_t>(asset->Meshes.size()));

			for (auto& submesh : asset->Meshes)
			{
				const MeshData& meshData = submesh->GetMeshData();

				auto& offsets = meshData.GetOffsets();
				auto& vertexData = meshData.GetVertexData();
				auto& indexData = meshData.GetIndices();
				uint32_t vertexCount = meshData.GetVertexCount();
				uint32_t indexCount = meshData.GetIndexCount();

				BS_ASSERT(writer.template serialize<uint32_t>(vertexCount));
				BS_ASSERT(writer.template serialize<uint32_t>(indexCount));

				// Write indices
				for (uint32_t i = 0; i < indexCount; i++)
					BS_ASSERT(writer.serialize_bits(indexData[i], 32U));

				// Write mappings and offsets
				BS_ASSERT(writer.template serialize<uint32_t>(static_cast<uint32_t>(offsets.size())));

				TrivialArray<uint32_t, Allocator::BaseAllocator<uint32_t>> values(Allocator::GetAllocator<uint32_t>());

				for (auto& [index, value] : offsets)
				{
					auto& [offset, type] = value;

					// Write mappings
					BS_ASSERT(writer.template serialize<int>(index));
					BS_ASSERT(writer.template serialize<ShaderDataType>(type));

					ShaderDataType baseType = ShaderDataTypeBaseType(type);
					uint32_t componentCount = ShaderDataTypeComponentCount(type);
					uint32_t dataTypeSize = ShaderDataTypeSize(baseType);
					uint32_t valueCount = vertexCount * componentCount;

					MH_ASSERT(dataTypeSize == 4, "Data types of sizes other than 4 not currently supported");

					// This copy should be optimized away
					values.resize(valueCount);
					std::memcpy(values.data(), vertexData.data() + offset, valueCount);

					// Write vertices
					for (uint32_t i = 0; i < valueCount; i++)
						BS_ASSERT(writer.serialize_bits(values[i], 32U));
				}
			}

			return true;
		}
		
		template<typename Stream>
		typename bitstream::utility::is_reading_t<Stream, Asset<Texture>>
		static Deserialize(Stream& reader) noexcept
		{
			MeshProps props;

			// Read materials
			size_t materialCount;
			BS_ASSERT(reader.template serialize<size_t>(materialCount));
			props.Materials.resize(materialCount);

			for (size_t i = 0; i < materialCount; ++i)
				BS_ASSERT(reader.template serialize<Asset<Material>>(props.Materials[i]));

			// IncludeNodes and IncludeBones
			BS_ASSERT(reader.template serialize<bool>(props.IncludeBones));
			BS_ASSERT(reader.template serialize<bool>(props.IncludeNodes));

			// TODO: Bones, nodes, hierarchy etc.

			// Read submeshes
			size_t submeshCount;
			BS_ASSERT(reader.template serialize<size_t>(submeshCount));

			for (size_t i = 0; i < submeshCount; ++i)
			{
				uint32_t vertexCount;
				uint32_t indexCount;

				BS_ASSERT(reader.template serialize<uint32_t>(vertexCount));
				BS_ASSERT(reader.template serialize<uint32_t>(indexCount));

				// Read indices
				TrivialArray<uint32_t, Allocator::BaseAllocator<uint32_t>> indices(Allocator::GetAllocator<uint32_t>());
				indices.resize(indexCount);
				for (uint32_t i = 0; i < indexCount; i++)
					BS_ASSERT(reader.serialize_bits(indices[i], 32U));

				MeshData meshData(vertexCount, std::move(indices));

				// Read mappings and offsets
				uint32_t offsetCount;
				BS_ASSERT(reader.template serialize<uint32_t>(offsetCount));

				TrivialArray<uint32_t, Allocator::BaseAllocator<uint32_t>> values(Allocator::GetAllocator<uint32_t>());

				for (size_t j = 0; j < offsetCount; ++j)
				{
					int index;
					ShaderDataType type;

					// Read mappings
					BS_ASSERT(reader.template serialize<int>(index));
					BS_ASSERT(reader.template serialize<ShaderDataType>(type));

					uint32_t componentCount = ShaderDataTypeComponentCount(type);
					uint32_t valueCount = vertexCount * componentCount;

					// This copy should be optimized away
					values.resize(valueCount);

					// Read vertices
					for (uint32_t i = 0; i < valueCount; i++)
						BS_ASSERT(reader.serialize_bits(values[i], 32U));

					meshData.SetVertices(index, type, values.data());
				}
			}

			// We can't actually create a Mesh object, since it's abstract...
			// TODO: Make it a complete type and move all the specific MeshProps etc into the editor

			return nullptr;
		}
	};
}