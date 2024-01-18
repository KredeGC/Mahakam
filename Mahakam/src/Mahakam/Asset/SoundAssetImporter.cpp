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

		const SoundProps& props = sound->GetProps();

		node["Filepath"] << sound->GetFilepath();
		node["Volume"] << props.Volume;
		node["Loop"] << props.Loop;
	}

	Asset<void> SoundAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		SoundProps props = DeserializeProps(node);

		return Sound::Create(filepath.string(), props);
	}

	SoundProps SoundAssetImporter::DeserializeProps(ryml::NodeRef& node)
	{
		SoundProps props;

		DeserializeYAMLNode(node, "Volume", props.Volume);
		DeserializeYAMLNode(node, "Loop", props.Loop);

		return props;
	}
}