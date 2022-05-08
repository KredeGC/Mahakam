#include "mhpch.h"
#include "SoundAssetImporter.h"

#include "Mahakam/Audio/Sound.h"

#include <imgui.h>

namespace Mahakam
{
	void SoundAssetImporter::OnWizardOpen(YAML::Node& node)
	{
		YAML::Node volumeNode = node["Volume"];
		soundProps.volume = 1.0f;
		if (volumeNode)
			soundProps.volume = volumeNode.as<float>();

		YAML::Node loopNode = node["Loop"];
		soundProps.loop = false;
		if (loopNode)
			soundProps.loop = loopNode.as<bool>();
	}

	void SoundAssetImporter::OnWizardRender()
	{
		ImGui::DragFloat("Sound Volume", &soundProps.volume, 0.01f, 0.0f);
		ImGui::Checkbox("Sound Looping", &soundProps.loop);
	}

	Ref<void> SoundAssetImporter::OnWizardImport(const std::filesystem::path& filepath)
	{
		return Sound::Create(filepath.string(), soundProps);
	}

	void SoundAssetImporter::Serialize(YAML::Emitter& emitter, Ref<void> asset)
	{
		Ref<Sound> sound = StaticCastRef<Sound>(asset);

		emitter << YAML::Key << "Filepath";
		emitter << YAML::Value << sound->GetFilepath();
		emitter << YAML::Key << "Volume";
		emitter << YAML::Value << sound->GetProps().volume;
		emitter << YAML::Key << "Loop";
		emitter << YAML::Value << sound->GetProps().loop;
	}

	Ref<void> SoundAssetImporter::Deserialize(YAML::Node& node)
	{
		YAML::Node filepathNode = node["Filepath"];
		std::string filepath;
		if (filepathNode)
			filepath = filepathNode.as<std::string>();

		YAML::Node volumeNode = node["Volume"];
		float volume = 1.0f;
		if (volumeNode)
			volume = volumeNode.as<float>();

		YAML::Node loopNode = node["Loop"];
		bool loop = false;
		if (loopNode)
			loop = loopNode.as<bool>();

		return Sound::Create(filepath, { volume, loop });
	}
}