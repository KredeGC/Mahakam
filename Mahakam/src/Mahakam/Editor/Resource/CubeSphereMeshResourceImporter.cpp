#include "Mahakam/mhpch.h"
#include "CubeSphereMeshResourceImporter.h"

#include "Mahakam/Editor/YAML/MeshSerialization.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

namespace Mahakam
{
	CubeSphereMeshResourceImporter::CubeSphereMeshResourceImporter() :
		ResourceImporter("Cube Sphere", ".cubesphere", "mesh"),
		m_Props()
	{
		m_ImporterProps.CreateMenu = true;
	}

	void CubeSphereMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		node >> m_Props;
	}

	void CubeSphereMeshResourceImporter::OnRender()
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

		ImGui::DragInt("Tessellation", &m_Props.Tessellation, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Checkbox("Invert shape", &m_Props.Invert);

		// TODO: Show a preview of the mesh
	}

	void CubeSphereMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		node << m_Props;
	}

	Asset<void> CubeSphereMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		CubeSphereMeshProps props;
		node >> props;

		return Mesh::Create(props);
	}
}