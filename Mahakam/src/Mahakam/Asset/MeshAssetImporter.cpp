#include "Mahakam/mhpch.h"
#include "MeshAssetImporter.h"

#include "Mahakam/Renderer/Mesh.h"

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
		m_MeshProps = {};

		YAML::Node primitiveNode = node["Primitive"];
		if (primitiveNode)
		{
			m_MeshProps.Primitive = (MeshPrimitive)primitiveNode.as<int>();
			if (m_MeshProps.Primitive == MeshPrimitive::Model)
				m_ImporterProps.NoFilepath = false;
			else
				m_ImporterProps.NoFilepath = true;
		}

		YAML::Node materialsNode = node["Materials"];
		if (materialsNode)
		{
			for (auto materialNode : materialsNode)
			{
				uint64_t materialID = materialNode.as<uint64_t>();
				Asset<Material> material = Asset<Material>(materialID);

				m_MeshProps.Materials.push_back(material);
			}
		}

		YAML::Node includeNodesNode = node["IncludeNodes"];
		if (includeNodesNode)
			m_MeshProps.IncludeNodes = includeNodesNode.as<bool>();

		YAML::Node includeBonesNode = node["IncludeBones"];
		if (includeBonesNode)
			m_MeshProps.IncludeBones = includeBonesNode.as<bool>();

		YAML::Node rowsNode = node["Rows"];
		if (rowsNode)
			m_MeshProps.Rows = rowsNode.as<int>();

		YAML::Node columnsNode = node["Columns"];
		if (columnsNode)
			m_MeshProps.Columns = columnsNode.as<int>();

		YAML::Node tessellationNode = node["Tessellation"];
		if (tessellationNode)
			m_MeshProps.Rows = tessellationNode.as<int>();

		YAML::Node invertNode = node["Invert"];
		if (invertNode)
			m_MeshProps.Invert = invertNode.as<bool>();
	}

	void MeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
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

		const char* primitives[]{ "Model", "Plane", "Cube", "Cube Sphere", "UV Sphere" };
		const char* currentPrimitive = primitives[(int)m_MeshProps.Primitive];

		if (ImGui::BeginCombo("Shape", currentPrimitive))
		{
			for (int i = 0; i < 5; i++)
			{
				bool selected = currentPrimitive == primitives[i];
				if (ImGui::Selectable(primitives[i], selected))
				{
					m_MeshProps.Primitive = (MeshPrimitive)i;
					if (m_MeshProps.Primitive == MeshPrimitive::Model)
						m_ImporterProps.NoFilepath = false;
					else
						m_ImporterProps.NoFilepath = true;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (m_MeshProps.Primitive == MeshPrimitive::Model)
		{
			ImGui::Checkbox("Include mesh nodes", &m_MeshProps.IncludeNodes);
			ImGui::Checkbox("Include mesh bones", &m_MeshProps.IncludeBones);

			if (ImGui::Button("Import animations"))
			{
				// TODO: Move ImportWizardPanel into Mahakam/Editor
				// TODO: Open AnimationAssetImporter in wizard
			}
		}
		else if (m_MeshProps.Primitive == MeshPrimitive::Plane || m_MeshProps.Primitive == MeshPrimitive::UVSphere)
		{
			ImGui::DragInt("Rows", &m_MeshProps.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragInt("Columns", &m_MeshProps.Columns, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
		}
		else if (m_MeshProps.Primitive == MeshPrimitive::Cube || m_MeshProps.Primitive == MeshPrimitive::CubeSphere)
		{
			ImGui::DragInt("Tessellation", &m_MeshProps.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Checkbox("Invert shape", &m_MeshProps.Invert);
		}

		// TODO: Show a preview of the mesh
	}

	void MeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Mesh> meshAsset = Asset<Mesh>(CreateRef<Mesh>(m_MeshProps));

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

		emitter << YAML::Key << "Primitive" << YAML::Value << (int)mesh->Props.Primitive;

		switch (mesh->Props.Primitive)
		{
		case MeshPrimitive::Model:
			emitter << YAML::Key << "IncludeNodes" << YAML::Value << mesh->Props.IncludeNodes;
			emitter << YAML::Key << "IncludeBones" << YAML::Value << mesh->Props.IncludeBones;
			break;
		case MeshPrimitive::Plane:
		case MeshPrimitive::UVSphere:
			emitter << YAML::Key << "Rows" << YAML::Value << mesh->Props.Rows;
			emitter << YAML::Key << "Columns" << YAML::Value << mesh->Props.Columns;
			break;
		case MeshPrimitive::Cube:
		case MeshPrimitive::CubeSphere:
			emitter << YAML::Key << "Tessellation" << YAML::Value << mesh->Props.Rows;
			emitter << YAML::Key << "Invert" << YAML::Value << mesh->Props.Invert;
			break;
		}
	}

	Ref<void> MeshAssetImporter::Deserialize(YAML::Node& node)
	{
		MeshProps meshProps;

		YAML::Node primitiveNode = node["Primitive"];
		if (primitiveNode)
			meshProps.Primitive = (MeshPrimitive)primitiveNode.as<int>();

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

		YAML::Node rowsNode = node["Rows"];
		if (rowsNode)
			meshProps.Rows = rowsNode.as<int>();

		YAML::Node columnsNode = node["Columns"];
		if (columnsNode)
			meshProps.Columns = columnsNode.as<int>();

		YAML::Node tessellationNode = node["Tessellation"];
		if (tessellationNode)
			meshProps.Rows = tessellationNode.as<int>();

		YAML::Node invertNode = node["Invert"];
		if (invertNode)
			meshProps.Invert = invertNode.as<bool>();

		YAML::Node filepathNode = node["Filepath"];
		switch (meshProps.Primitive)
		{
		case MeshPrimitive::Model:
			if (filepathNode)
			{
				std::string filepath = filepathNode.as<std::string>();

				return Mesh::LoadMesh(filepath, meshProps);
			}
		case MeshPrimitive::Plane:
			return CreateRef<Mesh>(SubMesh::CreatePlane(meshProps.Rows, meshProps.Columns), meshProps);
		case MeshPrimitive::Cube:
			return CreateRef<Mesh>(SubMesh::CreateCube(meshProps.Rows, meshProps.Invert), meshProps);
		case MeshPrimitive::CubeSphere:
			return CreateRef<Mesh>(SubMesh::CreateCubeSphere(meshProps.Rows, meshProps.Invert), meshProps);
		case MeshPrimitive::UVSphere:
			return CreateRef<Mesh>(SubMesh::CreateUVSphere(meshProps.Rows, meshProps.Columns), meshProps);
		}

		return nullptr;
	}
}