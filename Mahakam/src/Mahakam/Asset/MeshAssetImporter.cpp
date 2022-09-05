#include "Mahakam/mhpch.h"
#include "MeshAssetImporter.h"

#include "Mahakam/Renderer/Mesh.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	MeshAssetImporter::MeshAssetImporter()
	{
		m_ImporterProps.Extension = ".mesh";
	}

#ifndef MH_STANDALONE
	void MeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node)
	{

	}

	void MeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::Text("No options yet");
	}

	void MeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Mesh> meshAsset = Mesh::LoadMesh(filepath);

		meshAsset.Save(filepath, importPath);

		AssetDatabase::ReloadAsset(meshAsset.GetID());
	}
#endif

	void MeshAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		// TODO: Properties
	}

	Asset<void> MeshAssetImporter::Deserialize(YAML::Node& node)
	{
		YAML::Node filepathNode = node["Filepath"];
		if (filepathNode)
		{
			std::string filepath = filepathNode.as<std::string>();

			return Mesh::LoadMesh(filepath);
		}

		return nullptr;
	}
}