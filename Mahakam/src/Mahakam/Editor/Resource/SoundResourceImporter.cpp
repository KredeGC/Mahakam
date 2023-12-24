#include "Mahakam/mhpch.h"
#include "SoundResourceImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Audio/Sound.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	SoundResourceImporter::SoundResourceImporter() :
		ResourceImporter("Sound", ".sound"),
		m_Props() {}

	void SoundResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Filepath = filepath;
		m_Props = SoundProps();
	}

	void SoundResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		DeserializeYAMLNode(node, "Filepath", m_Filepath);

		m_Props = DeserializeProps(node);
	}

	void SoundResourceImporter::OnRender()
	{
		ImGui::DragFloat("Sound Volume", &m_Props.Volume, 0.01f, 0.0f);
		ImGui::Checkbox("Sound Looping", &m_Props.Loop);

		GUI::DrawDragDropField("File path", m_ImporterProps.Extension, m_Filepath);
	}

	void SoundResourceImporter::OnImport(ryml::NodeRef& node)
	{
		node["Filepath"] << m_Filepath;
		node["Volume"] << m_Props.Volume;
		node["Loop"] << m_Props.Loop;
	}

	Asset<void> SoundResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		SoundProps props = DeserializeProps(node);

		return Sound::Create(filepath.string(), props);
	}

	SoundProps SoundResourceImporter::DeserializeProps(ryml::NodeRef& node)
	{
		SoundProps props;

		DeserializeYAMLNode(node, "Volume", props.Volume);
		DeserializeYAMLNode(node, "Loop", props.Loop);

		return props;
	}
}