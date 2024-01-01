#include "Mahakam/mhpch.h"
#include "UVSphereMeshResourceImporter.h"

#include "Mahakam/Editor/Resource/ResourceRegistry.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

namespace Mahakam
{
	UVSphereMeshResourceImporter::UVSphereMeshResourceImporter() :
		ResourceImporter("UV Sphere", ".uvsphere"),
		m_Props()
	{
		m_ImporterProps.CreateMenu = true;
	}

	void UVSphereMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		m_Props = DeserializeProps(node);
	}

	void UVSphereMeshResourceImporter::OnRender()
	{
		int materialCount = (int)m_Props.Materials.size();
		if (ImGui::InputInt("Material count", &materialCount) && materialCount >= 0)
			m_Props.Materials.resize(materialCount);

		ImGui::Indent();
		for (size_t i = 0; i < m_Props.Materials.size(); i++)
		{
			std::filesystem::path importPath = m_Props.Materials[i].GetImportPath();
			if (GUI::DrawDragDropField("Material " + std::to_string(i), ".material", importPath))
				m_Props.Materials[i] = Asset<Material>(ResourceRegistry::GetImportInfo(importPath).ID);
		}
		ImGui::Unindent();

		ImGui::DragInt("Rows", &m_Props.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragInt("Columns", &m_Props.Columns, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);

		// TODO: Show a preview of the mesh
	}

	void UVSphereMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : m_Props.Materials)
			materialsNode.append_child() << material;

		node["Rows"] << m_Props.Rows;
		node["Columns"] << m_Props.Columns;
	}

	Asset<void> UVSphereMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		UVSphereMeshProps props = DeserializeProps(node);

		return UVSphereMesh::Create(props);
	}

	UVSphereMeshProps UVSphereMeshResourceImporter::DeserializeProps(ryml::NodeRef& node)
	{
		UVSphereMeshProps props;

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