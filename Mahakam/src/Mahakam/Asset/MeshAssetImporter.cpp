#include "Mahakam/mhpch.h"
#include "MeshAssetImporter.h"

#include "Mahakam/Renderer/Mesh.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	MeshAssetImporter::MeshAssetImporter()
	{
		Setup(m_ImporterProps, "Mesh", ".mesh");

		m_ImporterProps.CreateMenu = true;
	}

#ifndef MH_STANDALONE
	void MeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_MeshProps = {};

		if (!node.valid())
			return;

		if (node.has_child("Primitive"))
		{
			int primitive;
			node["Primitive"] >> primitive;
			m_MeshProps.Primitive = (MeshPrimitive)primitive;

			if (m_MeshProps.Primitive == MeshPrimitive::Model)
				m_ImporterProps.NoFilepath = false;
			else
				m_ImporterProps.NoFilepath = true;
		}

		if (node.has_child("Materials"))
		{
			uint64_t materialID;
			for (auto materialNode : node["Materials"])
			{
				materialNode >> materialID;
				Asset<Material> material = Asset<Material>(materialID);

				m_MeshProps.Materials.push_back(material);
			}
		}

		if (node.has_child("IncludeNodes"))
			node["IncludeNodes"] >> m_MeshProps.IncludeNodes;

		if (node.has_child("IncludeBones"))
			node["IncludeBones"] >> m_MeshProps.IncludeBones;

		if (node.has_child("Rows"))
			node["Rows"] >> m_MeshProps.Rows;

		if (node.has_child("Columns"))
			node["Columns"] >> m_MeshProps.Columns;

		if (node.has_child("Tessellation"))
			node["Tessellation"] >> m_MeshProps.Rows;

		if (node.has_child("Invert"))
			node["Invert"] >> m_MeshProps.Invert;
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
		Asset<Mesh> meshAsset;

		switch (m_MeshProps.Primitive)
		{
		case MeshPrimitive::Model:
			meshAsset = Mesh::LoadMesh(filepath, m_MeshProps);
			break;
		case MeshPrimitive::Plane:
			meshAsset = Mesh::Create(SubMesh::CreatePlane(m_MeshProps.Rows, m_MeshProps.Columns), m_MeshProps);
			break;
		case MeshPrimitive::Cube:
			meshAsset = Mesh::Create(SubMesh::CreateCube(m_MeshProps.Rows, m_MeshProps.Invert), m_MeshProps);
			break;
		case MeshPrimitive::CubeSphere:
			meshAsset = Mesh::Create(SubMesh::CreateCubeSphere(m_MeshProps.Rows, m_MeshProps.Invert), m_MeshProps);
			break;
		case MeshPrimitive::UVSphere:
			meshAsset = Mesh::Create(SubMesh::CreateUVSphere(m_MeshProps.Rows, m_MeshProps.Columns), m_MeshProps);
			break;
		}

		meshAsset.Save(m_ImporterProps.Extension, filepath, importPath);

		//AssetDatabase::ReloadAsset(meshAsset.GetID());
	}
#endif

	void MeshAssetImporter::Serialize(ryml::NodeRef& node, Asset<void> asset)
	{
		Asset<Mesh> mesh(asset);

		ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->Props.Materials)
			materialsNode.append_child() << material.GetID();

		node["Primitive"] << (int)mesh->Props.Primitive;

		switch (mesh->Props.Primitive)
		{
		case MeshPrimitive::Model:
			node["IncludeNodes"] << mesh->Props.IncludeNodes;
			node["IncludeBones"] << mesh->Props.IncludeBones;
			break;
		case MeshPrimitive::Plane:
		case MeshPrimitive::UVSphere:
			node["Rows"] << mesh->Props.Rows;
			node["Columns"] << mesh->Props.Columns;
			break;
		case MeshPrimitive::Cube:
		case MeshPrimitive::CubeSphere:
			node["Tessellation"] << mesh->Props.Rows;
			node["Invert"] << mesh->Props.Invert;
			break;
		}
	}

	Asset<void> MeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		MeshProps meshProps;

		if (node.has_child("Primitive"))
		{
			int primitive;
			node["Primitive"] >> primitive;
			meshProps.Primitive = (MeshPrimitive)primitive;
		}

		if (node.has_child("Materials"))
		{
			uint64_t materialID;
			for (auto materialNode : node["Materials"])
			{
				materialNode >> materialID;
				Asset<Material> material = Asset<Material>(materialID);

				meshProps.Materials.push_back(material);
			}
		}

		if (node.has_child("IncludeNodes"))
			node["IncludeNodes"] >> meshProps.IncludeNodes;

		if (node.has_child("IncludeBones"))
			node["IncludeBones"] >> meshProps.IncludeBones;

		if (node.has_child("Rows"))
			node["Rows"] >> meshProps.Rows;

		if (node.has_child("Columns"))
			node["Columns"] >> meshProps.Columns;

		if (node.has_child("Tessellation"))
			node["Tessellation"] >> meshProps.Rows;

		if (node.has_child("Invert"))
			node["Invert"] >> meshProps.Invert;

		switch (meshProps.Primitive)
		{
		case MeshPrimitive::Model:
			if (node.has_child("Filepath"))
			{
				std::string filepath;
				node["Filepath"] >> filepath;

				return Mesh::LoadMesh(filepath, meshProps);
			}
			return nullptr;
		case MeshPrimitive::Plane:
			return Mesh::Create(SubMesh::CreatePlane(meshProps.Rows, meshProps.Columns), meshProps);
		case MeshPrimitive::Cube:
			return Mesh::Create(SubMesh::CreateCube(meshProps.Rows, meshProps.Invert), meshProps);
		case MeshPrimitive::CubeSphere:
			return Mesh::Create(SubMesh::CreateCubeSphere(meshProps.Rows, meshProps.Invert), meshProps);
		case MeshPrimitive::UVSphere:
			return Mesh::Create(SubMesh::CreateUVSphere(meshProps.Rows, meshProps.Columns), meshProps);
		}

		return nullptr;
	}
}