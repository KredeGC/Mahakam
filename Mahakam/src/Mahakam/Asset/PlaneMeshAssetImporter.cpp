#include "Mahakam/mhpch.h"
#include "PlaneMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	PlaneMeshAssetImporter::PlaneMeshAssetImporter()
	{
		Setup(m_ImporterProps, "Plane", ".plane");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}

	void PlaneMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		PlaneMesh* mesh = static_cast<PlaneMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["Rows"] << mesh->Props.Rows;
		node["Columns"] << mesh->Props.Columns;
	}

	Asset<void> PlaneMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		PlaneMeshProps props;

		if (node.has_child("Materials"))
		{
			for (auto materialNode : node["Materials"])
			{
				Asset<Material> material;
				materialNode >> material;

				props.Materials.push_back(std::move(material));
			}
		}

		DeserializeYAMLNode(node, "Rows", props.Rows);
		DeserializeYAMLNode(node, "Columns", props.Columns);

		return PlaneMesh::Create(props);
	}

	PlaneMeshProps PlaneMeshAssetImporter::DeserializeProps(MeshPrimitive primitive, ryml::NodeRef& node)
	{
		PlaneMeshProps props;
		if (node.has_child("Rows"))
			node["Rows"] >> props.Rows;

		if (node.has_child("Columns"))
			node["Columns"] >> props.Columns;

		if (node.has_child("Materials"))
		{
			uint64_t materialID;
			for (auto materialNode : node["Materials"])
			{
				materialNode >> materialID;
				Asset<Material> material = Asset<Material>(materialID);

				props.Materials.push_back(material);
			}
		}

		return props;
	}
}