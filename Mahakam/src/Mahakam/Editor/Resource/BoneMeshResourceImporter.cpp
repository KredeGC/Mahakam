#include "Mahakam/mhpch.h"
#include "BoneMeshResourceImporter.h"

#include "Mahakam/Editor/Resource/ResourceRegistry.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	BoneMeshResourceImporter::BoneMeshResourceImporter() :
		ResourceImporter("Bone", ".bone"),
		m_Props() {}

	void BoneMeshResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Props = BoneMeshProps();
		m_Props.Filepath = filepath;
	}

	void BoneMeshResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		m_Props = DeserializeProps(node);
	}

	void BoneMeshResourceImporter::OnRender()
	{
		GUI::DrawDragDropField("File path", m_ImporterProps.Extension, m_Props.Filepath);

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

		SerializeYAMLNode(node, "Filepath", m_Props.Filepath);
		SerializeYAMLNode(node, "IncludeNodes", m_Props.IncludeNodes);
		SerializeYAMLNode(node, "IncludeBones", m_Props.IncludeBones);
	}

	Asset<void> BoneMeshResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		BoneMeshProps props = DeserializeProps(node);

		if (props.Filepath.empty())
			return nullptr;

		return BoneMesh::Create(props);
	}

	BoneMeshProps BoneMeshResourceImporter::DeserializeProps(ryml::NodeRef& node)
	{
		BoneMeshProps props;

		if (node.has_child("Materials"))
		{
			for (auto materialNode : node["Materials"])
			{
				Asset<Material> material;
				materialNode >> material;

				props.Materials.push_back(std::move(material));
			}
		}

		DeserializeYAMLNode(node, "Filepath", props.Filepath);
		DeserializeYAMLNode(node, "IncludeNodes", props.IncludeNodes);
		DeserializeYAMLNode(node, "IncludeBones", props.IncludeBones);

		return props;
	}
}