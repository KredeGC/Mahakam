#include "Mahakam/mhpch.h"
#include "PlaneMeshResourceImporter.h"

#include "Mahakam/Editor/Resource/ResourceRegistry.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

namespace Mahakam
{
	PlaneMeshResourceImporter::PlaneMeshResourceImporter() :
		ResourceImporter("Plane", ".plane"),
		m_Props()
	{
		m_ImporterProps.CreateMenu = true;
	}

	void PlaneMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		m_Props = DeserializeProps(node);
	}

	void PlaneMeshResourceImporter::OnRender()
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

		ImGui::DragInt("Rows", &m_Props.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragInt("Columns", &m_Props.Columns, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);

		// TODO: Show a preview of the mesh
	}

	void PlaneMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : m_Props.Materials)
			materialsNode.append_child() << material;

		node["Rows"] << m_Props.Rows;
		node["Columns"] << m_Props.Columns;
	}

	Asset<void> PlaneMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		PlaneMeshProps props = DeserializeProps(node);

		return PlaneMesh::Create(props);
	}

	PlaneMeshProps PlaneMeshResourceImporter::DeserializeProps(ryml::NodeRef& node)
	{
		PlaneMeshProps props;

		DeserializeYAMLNode(node, "Rows", props.Rows);
		DeserializeYAMLNode(node, "Columns", props.Columns);

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