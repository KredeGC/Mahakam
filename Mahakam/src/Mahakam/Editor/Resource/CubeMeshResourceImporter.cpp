#include "Mahakam/mhpch.h"
#include "CubeMeshResourceImporter.h"

#include "Mahakam/Editor/Resource/ResourceRegistry.h"
#include "Mahakam/Editor/YAML/AssetSerialization.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

namespace Mahakam
{
	CubeMeshResourceImporter::CubeMeshResourceImporter() :
		ResourceImporter("Cube", ".cube", "mesh"),
		m_Props()
	{
		m_ImporterProps.CreateMenu = true;
	}

	void CubeMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		m_Props = DeserializeProps(node);
	}

	void CubeMeshResourceImporter::OnRender()
	{
		int materialCount = (int)m_Props.Materials.size();
		if (ImGui::InputInt("Material count", &materialCount) && materialCount >= 0)
			m_Props.Materials.resize(materialCount);

		ImGui::Indent();
		for (size_t i = 0; i < m_Props.Materials.size(); i++)
		{
			GUI::DrawDragDropAsset("Material " + std::to_string(i), m_Props.Materials[i], ".material");
		}
		ImGui::Unindent();

		ImGui::DragInt("Tessellation", &m_Props.Tessellation, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Checkbox("Invert shape", &m_Props.Invert);

		// TODO: Show a preview of the mesh
	}

	void CubeMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : m_Props.Materials)
			materialsNode.append_child() << material;

		node["Tessellation"] << m_Props.Tessellation;
		node["Invert"] << m_Props.Invert;
	}

	Asset<void> CubeMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		CubeMeshProps props = DeserializeProps(node);

		return CubeMesh::Create(props);
	}

	CubeMeshProps CubeMeshResourceImporter::DeserializeProps(ryml::NodeRef& node)
	{
		CubeMeshProps props;

		DeserializeYAMLNode(node, "Tessellation", props.Tessellation);
		DeserializeYAMLNode(node, "Invert", props.Invert);

		if (node.has_child("Materials"))
		{
			for (auto materialNode : node["Materials"])
			{
				Asset<Material> material;
				materialNode >> material;

				props.Materials.push_back(std::move(material));
			}
		}

		return props;
	}
}