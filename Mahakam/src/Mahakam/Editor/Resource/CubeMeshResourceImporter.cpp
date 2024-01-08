#include "Mahakam/mhpch.h"
#include "CubeMeshResourceImporter.h"

#include "Mahakam/Editor/YAML/AssetSerialization.h"
#include "Mahakam/Editor/YAML/MeshSerialization.h"

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
		node >> m_Props;
	}

	void CubeMeshResourceImporter::OnRender()
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

	void CubeMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		node << m_Props;
	}

	Asset<void> CubeMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		CubeMeshProps props;
		node >> props;

		return Mesh::Create(props);
	}
}