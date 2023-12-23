#include "Mahakam/mhpch.h"
#include "CubeSphereMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	CubeSphereMeshAssetImporter::CubeSphereMeshAssetImporter()
	{
		Setup(m_ImporterProps, "CubeSphere", ".cubesphere");

		m_ImporterProps.CreateMenu = true;
		m_ImporterProps.NoFilepath = true;
	}

#ifndef MH_STANDALONE
	void CubeSphereMeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_MeshProps = CubeSphereMeshProps();

		if (!node.valid())
			return;

		m_MeshProps = DeserializeProps(node);
	}

	void CubeSphereMeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
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

	void CubeSphereMeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Mesh> meshAsset = CubeSphereMesh::Create(m_MeshProps);

		meshAsset.Save(m_ImporterProps.Extension, importPath);
	}
#endif

	void CubeSphereMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		CubeSphereMesh* mesh = static_cast<CubeSphereMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["Tessellation"] << mesh->Props.Tessellation;
		node["Invert"] << mesh->Props.Invert;
	}

	Asset<void> CubeSphereMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		CubeSphereMeshProps props = DeserializeProps(node);

		return CubeSphereMesh::Create(props);
	}

	CubeSphereMeshProps CubeSphereMeshAssetImporter::DeserializeProps(ryml::NodeRef& node)
	{
		CubeSphereMeshProps props;

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