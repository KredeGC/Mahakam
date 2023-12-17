#pragma once

#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/Texture.h"

#include "Mahakam/Serialization/BinarySerialization.h"
#include "Mahakam/Serialization/YAMLSerialization.h"

#include <bitstream.h>
#include <ryml/rapidyaml-0.4.1.hpp>

#include <cstdint>

namespace Mahakam
{
	template<typename>
	struct AssetSerializeTraits;

	template<>
	struct AssetSerializeTraits<Texture>
	{
		static void Serialize(ryml::NodeRef& node, Texture* asset) {}

		static Asset<Texture> Deserialize(ryml::NodeRef& node) { return nullptr; }

		static void Build(uint8_t* buffer, Texture* asset) {}

		static Asset<Texture> Read(uint8_t* buffer) { return nullptr; }
	};

	template<>
	struct AssetSerializeTraits<Mesh>
	{
		static void Serialize(ryml::NodeRef& node, Mesh* asset)
		{
			ryml::NodeRef materialsNode = node["Materials"];
			materialsNode |= ryml::SEQ;

			for (auto& material : asset->GetProps().Materials)
				materialsNode.append_child() << material;

			node["Primitive"] << (int)asset->Primitive;

			switch (asset->Primitive)
			{
			case MeshPrimitive::Model:
			{
				BoneMesh* boneMesh = static_cast<BoneMesh*>(asset);
				node["IncludeNodes"] << boneMesh->Props.IncludeNodes;
				node["IncludeBones"] << boneMesh->Props.IncludeBones;
				break;
			}
			case MeshPrimitive::Plane:
			{
				PlaneMesh* planeMesh = static_cast<PlaneMesh*>(asset);
				node["Rows"] << planeMesh->Props.Rows;
				node["Columns"] << planeMesh->Props.Columns;
				break;
			}
			case MeshPrimitive::Cube:
			{
				CubeMesh* cubeMesh = static_cast<CubeMesh*>(asset);
				node["Tessellation"] << cubeMesh->Props.Tessellation;
				node["Invert"] << cubeMesh->Props.Invert;
				break;
			}
			case MeshPrimitive::CubeSphere:
			{
				CubeSphereMesh* sphereMesh = static_cast<CubeSphereMesh*>(asset);
				node["Tessellation"] << sphereMesh->Props.Tessellation;
				node["Invert"] << sphereMesh->Props.Invert;
				break;
			}
			case MeshPrimitive::UVSphere:
			{
				UVSphereMesh* sphereMesh = static_cast<UVSphereMesh*>(asset);
				node["Rows"] << sphereMesh->Props.Rows;
				node["Columns"] << sphereMesh->Props.Columns;
				break;
			}
			default:
				MH_WARN("Unsupported Mesh primitive");
				break;
			}
		}

		static Asset<Mesh> Deserialize(ryml::NodeRef& node)
		{
			MeshPrimitive primitive;

			int primitiveInt;
			if (!DeserializeYAMLNode(node, "Primitive", primitiveInt))
				return nullptr;

			primitive = (MeshPrimitive)primitiveInt;

			std::vector<Asset<Material>> materials;
			if (node.has_child("Materials"))
			{
				for (auto materialNode : node["Materials"])
				{
					Asset<Material> material;
					materialNode >> material;

					materials.push_back(material);
				}
			}

			switch (primitive)
			{
			case MeshPrimitive::Model:
			{
				BoneMeshProps props;
				props.Materials = std::move(materials);

				std::string filepath;
				if (DeserializeYAMLNode(node, "Filepath", filepath))
					props.Filepath = filepath;

				DeserializeYAMLNode(node, "IncludeNodes", props.IncludeNodes);
				DeserializeYAMLNode(node, "IncludeBones", props.IncludeBones);

				return BoneMesh::Create(props);
			}
			case MeshPrimitive::Plane:
			{
				PlaneMeshProps props;
				props.Materials = materials;

				DeserializeYAMLNode(node, "Rows", props.Rows);
				DeserializeYAMLNode(node, "Columns", props.Columns);

				return PlaneMesh::Create(props);
			}
			case MeshPrimitive::Cube:
			{
				CubeMeshProps props;
				props.Materials = materials;

				DeserializeYAMLNode(node, "Tessellation", props.Tessellation);
				DeserializeYAMLNode(node, "Invert", props.Invert);

				return CubeMesh::Create(props);
			}
			case MeshPrimitive::CubeSphere:
			{
				CubeSphereMeshProps props;
				props.Materials = materials;

				DeserializeYAMLNode(node, "Tessellation", props.Tessellation);
				DeserializeYAMLNode(node, "Invert", props.Invert);

				return CubeSphereMesh::Create(props);
			}
			case MeshPrimitive::UVSphere:
			{
				UVSphereMeshProps props;
				props.Materials = materials;

				DeserializeYAMLNode(node, "Rows", props.Rows);
				DeserializeYAMLNode(node, "Columns", props.Columns);

				return UVSphereMesh::Create(props);
			}
			}

			MH_WARN("Unsupported Mesh primitive");

			return nullptr;
		}

		template<typename Stream>
		typename bitstream::utility::is_writing_t<Stream>
		static Build(Stream& writer, Mesh* asset) noexcept
		{
			// Write materials
			BS_ASSERT(writer.template serialize<size_t>(asset->GetProps().Materials.size()));

			for (auto& material : asset->GetProps().Materials)
				BS_ASSERT(writer.template serialize<Asset<Material>>(material));

			// TODO: IncludeNodes and IncludeBones

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
		static Read(Stream& reader) noexcept
		{
			MeshProps props;

			// Read materials
			size_t materialCount;
			BS_ASSERT(reader.template serialize<size_t>(materialCount));
			props.Materials.resize(materialCount);

			for (size_t i = 0; i < materialCount; ++i)
				BS_ASSERT(reader.template serialize<Asset<Material>>(props.Materials[i]));

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