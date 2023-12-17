#include "Mahakam/mhpch.h"
#include "CubeMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	CubeMeshAssetImporter::CubeMeshAssetImporter()
	{
		Setup(m_ImporterProps, "Cube", ".cube");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}

#ifndef MH_STANDALONE
	void CubeMeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_MeshProps = CubeMeshProps();

		if (!node.valid())
			return;

		m_MeshProps = DeserializeProps(node);
	}

	void CubeMeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
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

		ImGui::DragInt("Rows", &m_MeshProps.Tessellation, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Checkbox("Invert shape", &m_MeshProps.Invert);

		// TODO: Show a preview of the mesh
	}

	void CubeMeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Mesh> meshAsset = CubeMesh::Create(m_MeshProps);

		meshAsset.Save(m_ImporterProps.Extension, filepath, importPath);
	}
#endif

	void CubeMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		CubeMesh* mesh = static_cast<CubeMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["Tessellation"] << mesh->Props.Tessellation;
		node["Invert"] << mesh->Props.Invert;
	}

	Asset<void> CubeMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		CubeMeshProps props = DeserializeProps(node);

		return CubeMesh::Create(props);
	}

	CubeMeshProps CubeMeshAssetImporter::DeserializeProps(ryml::NodeRef& node)
	{
		CubeMeshProps props;

		if (node.has_child("Materials"))
		{
			for (auto materialNode : node["Materials"])
			{
				Asset<Material> material;
				materialNode >> material;

				props.Materials.push_back(std::move(material));
			}
		}

		DeserializeYAMLNode(node, "Tessellation", props.Tessellation);
		DeserializeYAMLNode(node, "Invert", props.Invert);

		return props;
	}
}