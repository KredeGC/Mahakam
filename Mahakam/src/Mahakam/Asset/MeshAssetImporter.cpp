#include "Mahakam/mhpch.h"
#include "MeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/Renderer/Mesh.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	MeshAssetImporter::MeshAssetImporter()
	{
		Setup(m_ImporterProps, "Mesh", ".mesh");

		m_ImporterProps.CreateMenu = true;

#ifndef MH_STANDALONE
		m_Primitive = MeshPrimitive::Model;
#endif
	}

#ifndef MH_STANDALONE
	void MeshAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_Primitive = MeshPrimitive::Model;
		m_MeshProps = CreateScope<BoneMeshProps>();

		if (!node.valid())
			return;

		if (node.has_child("Primitive"))
		{
			int primitiveInt;
			node["Primitive"] >> primitiveInt;
			m_Primitive = (MeshPrimitive)primitiveInt;

			if (m_Primitive == MeshPrimitive::Model)
				m_ImporterProps.NoFilepath = false;
			else
				m_ImporterProps.NoFilepath = true;

			m_MeshProps = DeserializeProps(m_Primitive, node);
		}
	}

	void MeshAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		int materialCount = (int)m_MeshProps->Materials.size();
		if (ImGui::InputInt("Material count", &materialCount) && materialCount >= 0)
			m_MeshProps->Materials.resize(materialCount);

		ImGui::Indent();
		for (size_t i = 0; i < m_MeshProps->Materials.size(); i++)
		{
			std::filesystem::path importPath = m_MeshProps->Materials[i].GetImportPath();
			if (GUI::DrawDragDropField("Material " + std::to_string(i), ".material", importPath))
				m_MeshProps->Materials[i] = Asset<Material>(importPath);
		}
		ImGui::Unindent();

		const char* primitives[]{ "Model", "Plane", "Cube", "Cube Sphere", "UV Sphere" };
		const char* currentPrimitive = primitives[(int)m_Primitive];

		if (ImGui::BeginCombo("Shape", currentPrimitive))
		{
			for (int i = 0; i < 5; i++)
			{
				bool selected = currentPrimitive == primitives[i];
				if (ImGui::Selectable(primitives[i], selected))
				{
					m_Primitive = (MeshPrimitive)i;
					if (m_Primitive == MeshPrimitive::Model)
						m_ImporterProps.NoFilepath = false;
					else
						m_ImporterProps.NoFilepath = true;

					Scope<MeshProps> props;
					switch (m_Primitive)
					{
					case MeshPrimitive::Model:
						props = CreateScope<BoneMeshProps>();
						break;
					case MeshPrimitive::Plane:
						props = CreateScope<PlaneMeshProps>();
						break;
					case MeshPrimitive::Cube:
						props = CreateScope<CubeMeshProps>();
						break;
					case MeshPrimitive::CubeSphere:
						props = CreateScope<CubeSphereMeshProps>();
						break;
					case MeshPrimitive::UVSphere:
						props = CreateScope<UVSphereMeshProps>();
						break;
					default:
						MH_WARN("Unsupported Mesh primitive");
						break;
					}

					props->Materials = m_MeshProps->Materials;
					m_MeshProps = std::move(props);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		switch (m_Primitive)
		{
		case MeshPrimitive::Model:
		{
			BoneMeshProps& props = static_cast<BoneMeshProps&>(*m_MeshProps.get());
			ImGui::Checkbox("Include mesh nodes", &props.IncludeNodes);
			ImGui::Checkbox("Include mesh bones", &props.IncludeBones);
			break;
		}
		case MeshPrimitive::Plane:
		{
			PlaneMeshProps& props = static_cast<PlaneMeshProps&>(*m_MeshProps.get());
			ImGui::DragInt("Rows", &props.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragInt("Columns", &props.Columns, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			break;
		}
		case MeshPrimitive::Cube:
		{
			CubeMeshProps& props = static_cast<CubeMeshProps&>(*m_MeshProps.get());
			ImGui::DragInt("Rows", &props.Tessellation, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Checkbox("Invert shape", &props.Invert);
			break;
		}
		case MeshPrimitive::CubeSphere:
		{
			CubeSphereMeshProps& props = static_cast<CubeSphereMeshProps&>(*m_MeshProps.get());
			ImGui::DragInt("Rows", &props.Tessellation, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Checkbox("Invert shape", &props.Invert);
			break;
		}
		case MeshPrimitive::UVSphere:
		{
			UVSphereMeshProps& props = static_cast<UVSphereMeshProps&>(*m_MeshProps.get());
			ImGui::DragInt("Rows", &props.Rows, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragInt("Columns", &props.Columns, 1.0f, 0, std::numeric_limits<int>::max(), "%d", ImGuiSliderFlags_AlwaysClamp);
			break;
		}
		default:
			MH_WARN("Unsupported Mesh primitive");
			break;
		}

		// TODO: Show a preview of the mesh
	}

	void MeshAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Mesh> meshAsset;

		switch (m_Primitive)
		{
		case MeshPrimitive::Model:
			meshAsset = BoneMesh::Create(static_cast<BoneMeshProps&>(*m_MeshProps.get()));
			break;
		case MeshPrimitive::Plane:
			meshAsset = PlaneMesh::Create(static_cast<PlaneMeshProps&>(*m_MeshProps.get()));
			break;
		case MeshPrimitive::Cube:
			meshAsset = CubeMesh::Create(static_cast<CubeMeshProps&>(*m_MeshProps.get()));
			break;
		case MeshPrimitive::CubeSphere:
			meshAsset = CubeSphereMesh::Create(static_cast<CubeSphereMeshProps&>(*m_MeshProps.get()));
			break;
		case MeshPrimitive::UVSphere:
			meshAsset = UVSphereMesh::Create(static_cast<UVSphereMeshProps&>(*m_MeshProps.get()));
			break;
		default:
			MH_WARN("Unsupported Mesh primitive");
			break;
		}

		meshAsset.Save(m_ImporterProps.Extension, filepath, importPath);
	}
#endif

	void MeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Mesh* mesh = static_cast<Mesh*>(asset);

		AssetSerializeTraits<Mesh>::Serialize(node, mesh);

		/*ryml::NodeRef materialsNode = node["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : mesh->GetProps().Materials)
			materialsNode.append_child() << material.GetID();

		node["Primitive"] << (int)mesh->Primitive;

		switch (mesh->Primitive)
		{
		case MeshPrimitive::Model:
		{
			BoneMesh* boneMesh = static_cast<BoneMesh*>(mesh);
			node["IncludeNodes"] << boneMesh->Props.IncludeNodes;
			node["IncludeBones"] << boneMesh->Props.IncludeBones;
			break;
		}
		case MeshPrimitive::Plane:
		{
			PlaneMesh* planeMesh = static_cast<PlaneMesh*>(mesh);
			node["Rows"] << planeMesh->Props.Rows;
			node["Columns"] << planeMesh->Props.Columns;
			break;
		}
		case MeshPrimitive::Cube:
		{
			CubeMesh* cubeMesh = static_cast<CubeMesh*>(mesh);
			node["Tessellation"] << cubeMesh->Props.Tessellation;
			node["Invert"] << cubeMesh->Props.Invert;
			break;
		}
		case MeshPrimitive::CubeSphere:
		{
			CubeSphereMesh* sphereMesh = static_cast<CubeSphereMesh*>(mesh);
			node["Tessellation"] << sphereMesh->Props.Tessellation;
			node["Invert"] << sphereMesh->Props.Invert;
			break;
		}
		case MeshPrimitive::UVSphere:
		{
			UVSphereMesh* sphereMesh = static_cast<UVSphereMesh*>(mesh);
			node["Rows"] << sphereMesh->Props.Rows;
			node["Columns"] << sphereMesh->Props.Columns;
			break;
		}
		default:
			MH_WARN("Unsupported Mesh primitive");
			break;
		}*/
	}

	Asset<void> MeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		return AssetSerializeTraits<Mesh>::Deserialize(node);

		/*MeshPrimitive primitive;
		if (node.has_child("Primitive"))
		{
			int primitiveInt;
			node["Primitive"] >> primitiveInt;
			primitive = (MeshPrimitive)primitiveInt;
		}
		else
		{
			return nullptr;
		}

		Scope<MeshProps> meshProps = DeserializeProps(primitive, node);

		switch (primitive)
		{
		case MeshPrimitive::Model:
			return BoneMesh::Create(static_cast<BoneMeshProps&>(*meshProps.get()));
		case MeshPrimitive::Plane:
			return PlaneMesh::Create(static_cast<PlaneMeshProps&>(*meshProps.get()));
		case MeshPrimitive::Cube:
			return CubeMesh::Create(static_cast<CubeMeshProps&>(*meshProps.get()));
		case MeshPrimitive::CubeSphere:
			return CubeSphereMesh::Create(static_cast<CubeSphereMeshProps&>(*meshProps.get()));
		case MeshPrimitive::UVSphere:
			return UVSphereMesh::Create(static_cast<UVSphereMeshProps&>(*meshProps.get()));
		}

		MH_WARN("Unsupported Mesh primitive");

		return nullptr;*/
	}

	Scope<MeshProps> MeshAssetImporter::DeserializeProps(MeshPrimitive primitive, ryml::NodeRef& node)
	{
		Scope<MeshProps> meshProps;

		switch (primitive)
		{
		case MeshPrimitive::Model:
		{
			Scope<BoneMeshProps> props = CreateScope<BoneMeshProps>();
			if (node.has_child("Filepath"))
			{
				std::string filepath;
				node["Filepath"] >> filepath;
				props->Filepath = filepath;
			}

			if (node.has_child("IncludeNodes"))
				node["IncludeNodes"] >> props->IncludeNodes;

			if (node.has_child("IncludeBones"))
				node["IncludeBones"] >> props->IncludeBones;

			meshProps = std::move(props);
			break;
		}
		case MeshPrimitive::Plane:
		{
			Scope<PlaneMeshProps> props = CreateScope<PlaneMeshProps>();
			if (node.has_child("Rows"))
				node["Rows"] >> props->Rows;

			if (node.has_child("Columns"))
				node["Columns"] >> props->Columns;

			meshProps = std::move(props);
			break;
		}
		case MeshPrimitive::Cube:
		{
			Scope<CubeMeshProps> props = CreateScope<CubeMeshProps>();
			if (node.has_child("Tessellation"))
				node["Tessellation"] >> props->Tessellation;

			if (node.has_child("Invert"))
				node["Invert"] >> props->Invert;

			meshProps = std::move(props);
			break;
		}
		case MeshPrimitive::CubeSphere:
		{
			Scope<CubeSphereMeshProps> props = CreateScope<CubeSphereMeshProps>();
			if (node.has_child("Tessellation"))
				node["Tessellation"] >> props->Tessellation;

			if (node.has_child("Invert"))
				node["Invert"] >> props->Invert;

			meshProps = std::move(props);
			break;
		}
		case MeshPrimitive::UVSphere:
		{
			Scope<UVSphereMeshProps> props = CreateScope<UVSphereMeshProps>();
			if (node.has_child("Rows"))
				node["Rows"] >> props->Rows;

			if (node.has_child("Columns"))
				node["Columns"] >> props->Columns;

			meshProps = std::move(props);
			break;
		}
		default:
			MH_WARN("Unsupported Mesh primitive");
			break;
		}

		if (node.has_child("Materials"))
		{
			uint64_t materialID;
			for (auto materialNode : node["Materials"])
			{
				materialNode >> materialID;
				Asset<Material> material = Asset<Material>(materialID);

				meshProps->Materials.push_back(material);
			}
		}

		return meshProps;
	}
}