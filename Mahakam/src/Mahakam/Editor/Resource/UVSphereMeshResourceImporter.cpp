#include "Mahakam/mhpch.h"
#include "UVSphereMeshResourceImporter.h"

#include "Mahakam/Editor/YAML/MeshSerialization.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

namespace Mahakam
{
	UVSphereMeshResourceImporter::UVSphereMeshResourceImporter() :
		ResourceImporter("UV Sphere", ".uvsphere", "mesh"),
		m_Props()
	{
		m_ImporterProps.CreateMenu = true;
	}

	void UVSphereMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		node >> m_Props;
	}

	void UVSphereMeshResourceImporter::OnRender()
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

	void UVSphereMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		node << m_Props;
	}

	Asset<void> UVSphereMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		UVSphereMeshProps props;
		node >> props;

		return Mesh::Create(props);
	}
}