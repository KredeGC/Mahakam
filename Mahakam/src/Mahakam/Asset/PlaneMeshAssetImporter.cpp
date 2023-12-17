#include "Mahakam/mhpch.h"
#include "PlaneMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	PlaneMeshAssetImporter::PlaneMeshAssetImporter()
	{
		Setup(m_ImporterProps, "Plane", ".plane");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}

#ifndef MH_STANDALONE
	void PlaneMeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_MeshProps = PlaneMeshProps();

		if (!node.valid())
			return;

		m_MeshProps = DeserializeProps(MeshPrimitive::Plane, node);
	}

	void PlaneMeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		int materialCount = (int)m_MeshProps.Materials.size();
		if (ImGui::InputInt("Material count", &materialCount) && materialCount >= 0)
			m_MeshProps.Materials.resize(materialCount);

		ImGui::Indent();
		for (size_t i = 0; i < m_MeshProps.Materials.size(); i++)
		{
			std::filesystem::path importPath = m_MeshProps.Materials[i].GetImportPath();
			if (GUI::DrawDragDropField("Material " + std::to_string(i), ".material", importPath))
				m_MeshProps.Materials[i] = Asset<Material>(importPath);
		}
		ImGui::Unindent();

		ImGui::DragInt("Rows", &m_MeshProps.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragInt("Columns", &m_MeshProps.Columns, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);

		// TODO: Show a preview of the mesh
	}

	void PlaneMeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Mesh> meshAsset = PlaneMesh::Create(m_MeshProps);

		meshAsset.Save(m_ImporterProps.Extension, filepath, importPath);
	}
#endif

	void PlaneMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		PlaneMesh* mesh = static_cast<PlaneMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["Rows"] << mesh->Props.Rows;
		node["Columns"] << mesh->Props.Columns;
	}

	Asset<void> PlaneMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		PlaneMeshProps props;

		if (node.has_child("Materials"))
		{
			for (auto materialNode : node["Materials"])
			{
				Asset<Material> material;
				materialNode >> material;

				props.Materials.push_back(std::move(material));
			}
		}

		DeserializeYAMLNode(node, "Rows", props.Rows);
		DeserializeYAMLNode(node, "Columns", props.Columns);

		return PlaneMesh::Create(props);
	}

	PlaneMeshProps PlaneMeshAssetImporter::DeserializeProps(MeshPrimitive primitive, ryml::NodeRef& node)
	{
		PlaneMeshProps props;
		if (node.has_child("Rows"))
			node["Rows"] >> props.Rows;

		if (node.has_child("Columns"))
			node["Columns"] >> props.Columns;

		if (node.has_child("Materials"))
		{
			uint64_t materialID;
			for (auto materialNode : node["Materials"])
			{
				materialNode >> materialID;
				Asset<Material> material = Asset<Material>(materialID);

				props.Materials.push_back(material);
			}
		}

		return props;
	}
}