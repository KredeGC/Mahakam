#include "Mahakam/mhpch.h"
#include "BoneMeshResourceImporter.h"

#include "Mahakam/Editor/Resource/ResourceRegistry.h"
#include "Mahakam/Editor/YAML/AssetSerialization.h"
#include "Mahakam/Editor/YAML/MeshSerialization.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	BoneMeshResourceImporter::BoneMeshResourceImporter() :
		ResourceImporter("Bone", ".bone", "mesh"),
		m_Props() {}

	void BoneMeshResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Props = MeshProps();
		m_Filepath = filepath;
	}

	void BoneMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		DeserializeYAMLNode(node, "Filepath", m_Filepath);

		node >> m_Props;
	}

	void BoneMeshResourceImporter::OnRender()
	{
		GUI::DrawDragDropField("File path", m_ImporterProps.Extension, m_Filepath);

		ImGui::Checkbox("Include mesh nodes", &m_Props.IncludeNodes);
		ImGui::Checkbox("Include mesh bones", &m_Props.IncludeBones);

		int materialCount = (int)m_Props.Materials.size();
		if (ImGui::InputInt("Material count", &materialCount) && materialCount >= 0)
			m_Props.Materials.resize(materialCount);

		ImGui::Indent();
		for (size_t i = 0; i < m_Props.Materials.size(); i++)
		{
			GUI::DrawDragDropAsset("Material " + std::to_string(i), m_Props.Materials[i], ".material");
		}
		ImGui::Unindent();

		// TODO: Show a preview of the mesh
	}

	void BoneMeshResourceImporter::OnImport(ryml::NodeRef& node)
	{
		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : m_Props.Materials)
			materialsNode.append_child() << material;

		SerializeYAMLNode(node, "Filepath", m_Filepath);
		SerializeYAMLNode(node, "IncludeNodes", m_Props.IncludeNodes);
		SerializeYAMLNode(node, "IncludeBones", m_Props.IncludeBones);
	}

	Asset<void> BoneMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		MeshProps props;
		node >> props;

		if (filepath.empty())
			return nullptr;

		return Mesh::Load(filepath, props);
	}
}