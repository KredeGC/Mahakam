#include "Mahakam/mhpch.h"
#include "AnimationResourceImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Animation.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	AnimationResourceImporter::AnimationResourceImporter() :
		ResourceImporter("Animation", ".anim"),
		m_Filepath(),
		m_Index(0) {}

	void AnimationResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Filepath = filepath;
		m_Index = 0;
	}

	void AnimationResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		DeserializeYAMLNode(node, "Filepath", m_Filepath);
		DeserializeYAMLNode(node, "Index", m_Index);
	}

	void AnimationResourceImporter::OnRender()
	{
		GUI::DrawIntDrag("Animation Index", m_Index, 1, 0, 0);

		// TODO: glTF and glb extensions
		GUI::DrawDragDropField("File path", m_ImporterProps.Extension, m_Filepath);
	}

	void AnimationResourceImporter::OnImport(ryml::NodeRef& node)
	{
		node["Filepath"] << m_Filepath;
		node["Index"] << m_Index;
	}

	Asset<void> AnimationResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		int index = 0;
		DeserializeYAMLNode(node, "Index", index);

		if (!filepath.empty())
			return Animation::Load(filepath, index);

		return nullptr;
	}
}