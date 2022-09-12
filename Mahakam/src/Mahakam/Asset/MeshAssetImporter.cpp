#include "Mahakam/mhpch.h"
#include "MeshAssetImporter.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	MeshAssetImporter::MeshAssetImporter()
	{
		m_ImporterProps.Extension = ".mesh";
		m_ImporterProps.CreateMenu = true;
	}

#ifndef MH_STANDALONE
	void MeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node)
	{

	}

	void MeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		int materialCount = (int)m_MeshProps.Materials.size();
		if (ImGui::InputInt("Material count", &materialCount))
			m_MeshProps.Materials.resize(materialCount);

		for (size_t i = 0; i < m_MeshProps.Materials.size(); i++)
		{
			std::filesystem::path importPath = m_MeshProps.Materials[i].GetImportPath();
			if (GUI::DrawDragDropField("Material " + std::to_string(i), ".material", importPath))
				m_MeshProps.Materials[i] = Asset<Material>(importPath);
		}
	}

	void MeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Mesh> meshAsset = Asset<Mesh>(Mesh::LoadMesh(filepath, m_MeshProps));

		meshAsset.Save(filepath, importPath);

		AssetDatabase::ReloadAsset(meshAsset.GetID());
	}
#endif

	void MeshAssetImporter::Serialize(YAML::Emitter& emitter, Ref<void> asset)
	{
		Ref<Mesh> mesh = StaticCastRef<Mesh>(asset);

		emitter << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;
		for (auto& material : mesh->Props.Materials)
		{
			emitter << YAML::Value << material.GetID();
		}
		emitter << YAML::EndSeq;

		emitter << YAML::Key << "IncludeNodes" << YAML::Value << mesh->Props.IncludeNodes;
		emitter << YAML::Key << "IncludeBones" << YAML::Value << mesh->Props.IncludeBones;
	}

	Ref<void> MeshAssetImporter::Deserialize(YAML::Node& node)
	{
		MeshProps meshProps;

		YAML::Node materialsNode = node["Materials"];
		if (materialsNode)
		{
			for (auto materialNode : materialsNode)
			{
				uint64_t materialID = materialNode.as<uint64_t>();
				Asset<Material> material = Asset<Material>(materialID);

				meshProps.Materials.push_back(material);
			}
		}

		YAML::Node includeNodesNode = node["IncludeNodes"];
		if (includeNodesNode)
			meshProps.IncludeNodes = includeNodesNode.as<bool>();

		YAML::Node includeBonesNode = node["IncludeBones"];
		if (includeBonesNode)
			meshProps.IncludeBones = includeBonesNode.as<bool>();

		YAML::Node filepathNode = node["Filepath"];
		if (filepathNode)
		{
			std::string filepath = filepathNode.as<std::string>();

			return Mesh::LoadMesh(filepath, meshProps);
		}

		return nullptr;
	}
}