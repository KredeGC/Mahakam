#include "Mahakam/mhpch.h"
#include "UVSphereMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/Editor/YAML/AssetSerialization.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	UVSphereMeshAssetImporter::UVSphereMeshAssetImporter()
	{
		Setup(m_ImporterProps, "UVSphere", ".uvsphere");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}


	void UVSphereMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		UVSphereMesh* mesh = static_cast<UVSphereMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["Rows"] << static_cast<UVSphereMeshProps&>(mesh->GetProps()).Rows;
		node["Columns"] << static_cast<UVSphereMeshProps&>(mesh->GetProps()).Columns;
	}

	Asset<void> UVSphereMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		UVSphereMeshProps props = DeserializeProps(node);

		return UVSphereMesh::Create(props);
	}

	UVSphereMeshProps UVSphereMeshAssetImporter::DeserializeProps(ryml::NodeRef& node)
	{
		UVSphereMeshProps props;

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

		return props;
	}
}