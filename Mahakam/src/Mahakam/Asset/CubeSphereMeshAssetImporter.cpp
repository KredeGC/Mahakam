#include "Mahakam/mhpch.h"
#include "CubeSphereMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	CubeSphereMeshAssetImporter::CubeSphereMeshAssetImporter()
	{
		Setup(m_ImporterProps, "CubeSphere", ".cubesphere");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}

	void CubeSphereMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		CubeSphereMesh* mesh = static_cast<CubeSphereMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["Tessellation"] << mesh->Props.Tessellation;
		node["Invert"] << mesh->Props.Invert;
	}

	Asset<void> CubeSphereMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		CubeSphereMeshProps props = DeserializeProps(node);

		return CubeSphereMesh::Create(props);
	}

	CubeSphereMeshProps CubeSphereMeshAssetImporter::DeserializeProps(ryml::NodeRef& node)
	{
		CubeSphereMeshProps props;

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