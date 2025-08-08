#include "Mahakam/mhpch.h"
#include "PlaneMeshResourceImporter.h"

#include "Mahakam/Editor/YAML/AssetSerialization.h"
#include "Mahakam/Editor/YAML/MeshSerialization.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

namespace Mahakam
{
	PlaneMeshResourceImporter::PlaneMeshResourceImporter() :
		ResourceImporter("Plane", ".plane", "mesh"),
		m_Props()
	{
		m_ImporterProps.CreateMenu = true;
	}

	void PlaneMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		node >> m_Props;
	}

	void PlaneMeshResourceImporter::OnRender()
	{
		int materialCount = (int)m_Props.Base.Materials.size();
		if (ImGui::InputInt("Material count", &materialCount) && materialCount >= 0)
			m_Props.Base.Materials.resize(materialCount);

		ImGui::Indent();
		for (size_t i = 0; i < m_Props.Base.Materials.size(); i++)
		{
			GUI::DrawDragDropAsset("Material " + std::to_string(i), m_Props.Base.Materials[i], ".material");
		}
		ImGui::Unindent();

		ImGui::DragInt("Rows", &m_Props.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragInt("Columns", &m_Props.Columns, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);

		// TODO: Show a preview of the mesh
	}

	void PlaneMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		node << m_Props;
	}

	Asset<void> PlaneMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		PlaneMeshProps props;
		node >> props;

		return Mesh::Create(props);
	}
}