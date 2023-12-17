#include "Mahakam/mhpch.h"
#include "BoneMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	BoneMeshAssetImporter::BoneMeshAssetImporter()
	{
		Setup(m_ImporterProps, "Bone", ".bone");
	}

#ifndef MH_STANDALONE
	void BoneMeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_MeshProps = BoneMeshProps();

		if (!node.valid())
			return;

		m_MeshProps = DeserializeProps(node);
	}

	void BoneMeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
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

		ImGui::Checkbox("Include mesh nodes", &m_MeshProps.IncludeNodes);
		ImGui::Checkbox("Include mesh bones", &m_MeshProps.IncludeBones);

		// TODO: Show a preview of the mesh
	}

	void BoneMeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		m_MeshProps.Filepath = filepath;

		Asset<Mesh> meshAsset = BoneMesh::Create(m_MeshProps);

		meshAsset.Save(m_ImporterProps.Extension, filepath, importPath);
	}
#endif

	void BoneMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		BoneMesh* mesh = static_cast<BoneMesh*>(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material;

		node["IncludeNodes"] << mesh->Props.IncludeNodes;
		node["IncludeBones"] << mesh->Props.IncludeBones;
	}

	Asset<void> BoneMeshAssetImporter::Deserialize(ryml::NodeRef& node)
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

		std::string filepath;
		if (DeserializeYAMLNode(node, "Filepath", filepath))
			props.Filepath = filepath;

		DeserializeYAMLNode(node, "IncludeNodes", props.IncludeNodes);
		DeserializeYAMLNode(node, "IncludeBones", props.IncludeBones);

		return BoneMesh::Create(props);
	}

	BoneMeshProps BoneMeshAssetImporter::DeserializeProps(ryml::NodeRef& node)
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

		std::string filepath;
		if (DeserializeYAMLNode(node, "Filepath", filepath))
			props.Filepath = filepath;

		DeserializeYAMLNode(node, "IncludeNodes", props.IncludeNodes);
		DeserializeYAMLNode(node, "IncludeBones", props.IncludeBones);

		return props;
	}
}