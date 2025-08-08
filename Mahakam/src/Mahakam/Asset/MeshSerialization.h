#pragma once

#include "AssetSerializeTraits.h"

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/MeshProps.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/ShaderUtility.h"

#include "Mahakam/BinarySerialization/AssetIDSerialization.h"
#include "Mahakam/BinarySerialization/ContainerSerialization.h"
#include "Mahakam/BinarySerialization/MathSerialization.h"
#include "Mahakam/BinarySerialization/StringSerialization.h"

namespace Mahakam::Serialization
{
	template<>
	struct SerializeTraits<MeshNode>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, MeshNode> node) noexcept
		{
			// TODO: Should node name even be serialized?
			MH_SER_ASSERT(stream.serialize(node.Name));

			MH_SER_ASSERT(stream.serialize(node.ID));
			MH_SER_ASSERT(stream.serialize(node.ParentID));
			MH_SER_ASSERT(stream.serialize(node.Offset));

			return true;
		}
	};

	template<>
	struct SerializeTraits<MeshProps>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, MeshProps> props) noexcept
		{
			return stream.serialize(props.Materials);
		}
	};

	template<>
	struct SerializeTraits<Ref<SubMesh>>
	{
		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, const Ref<SubMesh>& submesh) noexcept
		{
			const MeshData& meshData = submesh->GetMeshData();

			auto& offsets = meshData.GetOffsets();
			auto& vertexData = meshData.GetVertexData();
			auto& indexData = meshData.GetIndices();

			uint32_t vertexCount = meshData.GetVertexCount();
			uint32_t indexCount = meshData.GetIndexCount();

			MH_SER_ASSERT(writer.serialize(vertexCount));
			MH_SER_ASSERT(writer.serialize(indexCount));

			// Write indices
			for (uint32_t i = 0; i < indexCount; i++)
				MH_SER_ASSERT(writer.serialize(indexData[i]));

			// Write mappings and offsets
			MH_SER_ASSERT(writer.serialize(static_cast<uint32_t>(offsets.size())));

			for (auto& [index, value] : offsets)
			{
				auto& [offset, type] = value;

				// Write mappings
				MH_SER_ASSERT(writer.serialize(index));
				MH_SER_ASSERT(writer.serialize(type));

				uint32_t dataTypeSize = ShaderUtility::ShaderDataTypeSize(type);
				uint32_t valueCount = vertexCount * dataTypeSize;

				// This will write it in native endian order... Too bad
				MH_SER_ASSERT(writer.serialize_elements(vertexData.data() + offset, valueCount));
			}

			return true;
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& reader, Ref<SubMesh>& submesh) noexcept
		{
			uint32_t vertexCount;
			uint32_t indexCount;

			MH_SER_ASSERT(reader.serialize(vertexCount));
			MH_SER_ASSERT(reader.serialize(indexCount));

			// Read indices
			auto indices = CreateTrivialArray<uint32_t>();
			indices.resize(indexCount);
			for (uint32_t i = 0; i < indexCount; i++)
				MH_SER_ASSERT(reader.serialize(indices[i]));

			MeshData meshData = MeshData(vertexCount, std::move(indices));

			// Read mappings and offsets
			uint32_t offsetCount;
			MH_SER_ASSERT(reader.serialize(offsetCount));

			auto values = CreateTrivialArray<uint8_t>();

			for (size_t j = 0; j < offsetCount; ++j)
			{
				int index;
				ShaderDataType type;

				// Read mappings
				MH_SER_ASSERT(reader.serialize(index));
				MH_SER_ASSERT(reader.serialize(type));

				uint32_t dataTypeSize = ShaderUtility::ShaderDataTypeSize(type);
				uint32_t valueCount = vertexCount * dataTypeSize;

				// Read vertices
				values.resize(valueCount);
				MH_SER_ASSERT(reader.serialize_elements(values.data(), valueCount));

				meshData.SetVertices(index, type, values.data(), dataTypeSize);
			}

			submesh = SubMesh::Create(std::move(meshData));

			return true;
		}
	};

	template<>
	struct AssetSerializeTraits<Mesh>
	{
		template<typename Stream>
		static std::vector<Asset<void>> dependencies(Stream& reader) noexcept
		{
			MeshProps props;
			if (!reader.serialize(props))
				return {};

			std::vector<Asset<void>> assets;
			assets.reserve(props.Materials.size());
			for (auto& material : props.Materials)
			{
				assets.emplace_back(std::move(material));
			}

			return assets;
		}

		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Asset<Mesh>> mesh) noexcept
		{
			// Props
			if constexpr (Stream::writing)
			{
				MH_SER_ASSERT(stream.serialize(mesh->GetProps()));
			}
			else
			{
				MeshProps props;
				MH_SER_ASSERT(stream.serialize(props));

				mesh = CreateAsset<Mesh>(props);
			}

			// NodeHierarchy and bones
			MH_SER_ASSERT(stream.serialize(mesh->NodeHierarchy));
			MH_SER_ASSERT(stream.serialize(mesh->Skins));
			MH_SER_ASSERT(stream.serialize(mesh->SubMeshMap));
			MH_SER_ASSERT(stream.serialize(mesh->BoneMap));

			// Submeshes
			MH_SER_ASSERT(stream.serialize(mesh->Meshes));

			return true;
		}
	};
}