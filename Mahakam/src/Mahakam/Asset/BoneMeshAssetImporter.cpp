#include "Mahakam/mhpch.h"
#include "BoneMeshAssetImporter.h"

#include "AssetSerializeTraits.h"

#include "Mahakam/Editor/YAML/AssetSerialization.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Mesh.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <bitstream.h>
#include <imgui/imgui.h>

namespace Mahakam
{
	BoneMeshAssetImporter::BoneMeshAssetImporter()
	{
		Setup(m_ImporterProps, "Bone", ".bone");
	}

	void BoneMeshAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		
	}

	Asset<void> BoneMeshAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		BoneMeshProps props = DeserializeProps(node);

		return Mesh::Load(props.Filepath, props);
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

		DeserializeYAMLNode(node, "Filepath", props.Filepath);
		DeserializeYAMLNode(node, "IncludeNodes", props.IncludeNodes);
		DeserializeYAMLNode(node, "IncludeBones", props.IncludeBones);

		return props;
	}
}