#include "Mahakam/mhpch.h"
#include "CubeMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	CubeMeshAssetImporter::CubeMeshAssetImporter()
	{
		Setup(m_ImporterProps, "Cube", ".cube");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}

	void CubeMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		CubeMesh* mesh = static_cast<CubeMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["Tessellation"] << mesh->Props.Tessellation;
		node["Invert"] << mesh->Props.Invert;
	}

	Asset<void> CubeMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		CubeMeshProps props = DeserializeProps(node);

		return CubeMesh::Create(props);
	}

	CubeMeshProps CubeMeshAssetImporter::DeserializeProps(ryml::NodeRef& node)
	{
		CubeMeshProps props;

		if (node.has_child("Materials"))
		{
			for (auto materialNode : node["Materials"])
			{
				Asset<Material> material;
				materialNode >> material;

				props.Materials.push_back(std::move(material));
			}
		}

		DeserializeYAMLNode(node, "Tessellation", props.Tessellation);
		DeserializeYAMLNode(node, "Invert", props.Invert);

		return props;
	}
}