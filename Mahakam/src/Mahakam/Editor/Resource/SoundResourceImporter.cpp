#include "Mahakam/mhpch.h"
#include "SoundResourceImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Audio/Sound.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	SoundResourceImporter::SoundResourceImporter() :
		ResourceImporter("Sound", ".sound", "sound") {}

	void SoundResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Filepath = filepath;
	}

	void SoundResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		DeserializeYAMLNode(node, "Filepath", m_Filepath);
	}

	void SoundResourceImporter::OnRender()
	{
		GUI::DrawDragDropField("File path", m_ImporterProps.Extension, m_Filepath);
	}

	void SoundResourceImporter::OnImport(ryml::NodeRef& node)
	{
		node["Filepath"] << m_Filepath;
	}

	Asset<void> SoundResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		return Sound::Create(filepath.string());
	}
}