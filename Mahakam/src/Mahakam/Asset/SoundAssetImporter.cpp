#include "Mahakam/mhpch.h"
#include "SoundAssetImporter.h"

#include "Mahakam/Audio/Sound.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	SoundAssetImporter::SoundAssetImporter()
	{
		Setup(m_ImporterProps, "Sound", ".sound");
	}

	void SoundAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Sound* sound = static_cast<Sound*>(asset);

		node["Filepath"] << sound->GetFilepath();
	}

	Asset<void> SoundAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		return Sound::Create(filepath.string());
	}
}