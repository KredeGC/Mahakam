#pragma once

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/MeshProps.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/ShaderUtility.h"

#include "AssetSerialization.h"
#include "ContainerSerialization.h"
#include "MathSerialization.h"

#include <bitstream.h>

namespace bitstream
{
	template<>
	struct serialize_traits<Mahakam::MeshNode>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Mahakam::MeshNode> node) noexcept
		{
			// TODO: Should node name even be serialized?
			BS_ASSERT(stream.serialize(node.Name, 256U));

			BS_ASSERT(stream.serialize(node.ID));
			BS_ASSERT(stream.serialize(node.ParentID));
			BS_ASSERT(stream.serialize(node.Offset));

			return true;
		}
	};

	template<>
	struct serialize_traits<Mahakam::MeshProps>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Mahakam::MeshProps> props) noexcept
		{
			size_t materialCount = props.Materials.size();
			BS_ASSERT(stream.serialize(materialCount));

			if constexpr (Stream::reading)
				props.Materials.resize(materialCount);

			for (size_t i = 0; i < materialCount; ++i)
			{
				BS_ASSERT(stream.serialize(props.Materials[i]));
			}

			return true;
		}
	};

	template<>
	struct serialize_traits<Mahakam::Ref<Mahakam::SubMesh>>
	{
		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, const Mahakam::Ref<Mahakam::SubMesh>& submesh) noexcept
		{
			const Mahakam::MeshData& meshData = submesh->GetMeshData();

			auto& offsets = meshData.GetOffsets();
			auto& vertexData = meshData.GetVertexData();
			auto& indexData = meshData.GetIndices();

			uint32_t vertexCount = meshData.GetVertexCount();
			uint32_t indexCount = meshData.GetIndexCount();

			BS_ASSERT(writer.serialize(vertexCount));
			BS_ASSERT(writer.serialize(indexCount));

			// Write indices
			for (uint32_t i = 0; i < indexCount; i++)
				BS_ASSERT(writer.serialize_bits(indexData[i], 32U));

			// Write mappings and offsets
			BS_ASSERT(writer.serialize(static_cast<uint32_t>(offsets.size())));

			for (auto& [index, value] : offsets)
			{
				auto& [offset, type] = value;

				// Write mappings
				BS_ASSERT(writer.serialize(index));
				BS_ASSERT(writer.serialize(type));

				uint32_t dataTypeSize = Mahakam::ShaderUtility::ShaderDataTypeSize(type);
				uint32_t valueCount = vertexCount * dataTypeSize;

				// This will write it in native endian order... Too bad
				BS_ASSERT(writer.serialize_bytes(vertexData.data() + offset, valueCount * 8U));
			}

			return true;
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& reader, Mahakam::Ref<Mahakam::SubMesh>& submesh) noexcept
		{
			uint32_t vertexCount;
			uint32_t indexCount;

			BS_ASSERT(reader.serialize(vertexCount));
			BS_ASSERT(reader.serialize(indexCount));

			// Read indices
			Mahakam::TrivialArray<uint32_t, Mahakam::Allocator::BaseAllocator<uint32_t>> indices(Mahakam::Allocator::GetAllocator<uint32_t>());
			indices.resize(indexCount);
			for (uint32_t i = 0; i < indexCount; i++)
				BS_ASSERT(reader.serialize_bits(indices[i], 32U));

			Mahakam::MeshData meshData = Mahakam::MeshData(vertexCount, std::move(indices));

			// Read mappings and offsets
			uint32_t offsetCount;
			BS_ASSERT(reader.serialize(offsetCount));

			Mahakam::TrivialArray<uint8_t, Mahakam::Allocator::BaseAllocator<uint8_t>> values(Mahakam::Allocator::GetAllocator<uint32_t>());

			for (size_t j = 0; j < offsetCount; ++j)
			{
				int index;
				Mahakam::ShaderDataType type;

				// Read mappings
				BS_ASSERT(reader.serialize(index));
				BS_ASSERT(reader.serialize(type));

				uint32_t dataTypeSize = Mahakam::ShaderUtility::ShaderDataTypeSize(type);
				uint32_t valueCount = vertexCount * dataTypeSize;

				// Read vertices
				values.resize(valueCount);
				BS_ASSERT(reader.serialize_bytes(values.data(), valueCount * 8U));

				meshData.SetVertices(index, type, values.data(), dataTypeSize);
			}

			submesh = Mahakam::SubMesh::Create(std::move(meshData));

			return true;
		}
	};

	template<>
	struct serialize_traits<Mahakam::Mesh>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Mahakam::Asset<Mahakam::Mesh>> mesh) noexcept
		{
			using namespace Mahakam;

			// Props
			if constexpr (Stream::writing)
			{
				BS_ASSERT(stream.serialize(mesh->GetProps()));
			}
			else
			{
				MeshProps props;
				BS_ASSERT(stream.serialize(props));

				mesh = CreateAsset<Mesh>(props);
			}

			// NodeHierarchy and bones
			BS_ASSERT(stream.serialize(mesh->NodeHierarchy));
			BS_ASSERT(stream.serialize(mesh->Skins));
			BS_ASSERT(stream.serialize(mesh->SubMeshMap));
			BS_ASSERT(stream.serialize(mesh->BoneMap));

			// Submeshes
			BS_ASSERT(stream.serialize(mesh->Meshes));

			return true;
		}
	};
}