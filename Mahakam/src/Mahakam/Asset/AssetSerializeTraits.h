#pragma once

#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/Texture.h"

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
				materialsNode.append_child() << material.GetID();

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
			if (node.has_child("Primitive"))
			{
				int primitiveInt;
				node["Primitive"] >> primitiveInt;
				primitive = (MeshPrimitive)primitiveInt;
			}
			else
			{
				return nullptr;
			}

			std::vector<Asset<Material>> materials;
			if (node.has_child("Materials"))
			{
				uint64_t materialID;
				for (auto materialNode : node["Materials"])
				{
					materialNode >> materialID;
					Asset<Material> material = Asset<Material>(materialID);

					materials.push_back(material);
				}
			}

			switch (primitive)
			{
			case MeshPrimitive::Model:
			{
				BoneMeshProps props;
				props.Materials = materials;

				if (node.has_child("Filepath"))
				{
					std::string filepath;
					node["Filepath"] >> filepath;
					props.Filepath = filepath;
				}

				if (node.has_child("IncludeNodes"))
					node["IncludeNodes"] >> props.IncludeNodes;

				if (node.has_child("IncludeBones"))
					node["IncludeBones"] >> props.IncludeBones;

				return BoneMesh::Create(props);
			}
			case MeshPrimitive::Plane:
			{
				PlaneMeshProps props;
				props.Materials = materials;

				if (node.has_child("Rows"))
					node["Rows"] >> props.Rows;

				if (node.has_child("Columns"))
					node["Columns"] >> props.Columns;

				return PlaneMesh::Create(props);
			}
			case MeshPrimitive::Cube:
			{
				CubeMeshProps props;
				props.Materials = materials;

				if (node.has_child("Tessellation"))
					node["Tessellation"] >> props.Tessellation;

				if (node.has_child("Invert"))
					node["Invert"] >> props.Invert;

				return CubeMesh::Create(props);
			}
			case MeshPrimitive::CubeSphere:
			{
				CubeSphereMeshProps props;
				props.Materials = materials;

				if (node.has_child("Tessellation"))
					node["Tessellation"] >> props.Tessellation;

				if (node.has_child("Invert"))
					node["Invert"] >> props.Invert;

				return CubeSphereMesh::Create(props);
			}
			case MeshPrimitive::UVSphere:
			{
				UVSphereMeshProps props;
				props.Materials = materials;

				if (node.has_child("Rows"))
					node["Rows"] >> props.Rows;

				if (node.has_child("Columns"))
					node["Columns"] >> props.Columns;

				return UVSphereMesh::Create(props);
			}
			}

			MH_WARN("Unsupported Mesh primitive");

			return nullptr;
		}
	};
}