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

#ifndef MH_STANDALONE
	void SoundAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_Props = DeserializeProps(node);
	}

	void SoundAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::DragFloat("Sound Volume", &m_Props.Volume, 0.01f, 0.0f);
		ImGui::Checkbox("Sound Looping", &m_Props.Loop);
	}

	void SoundAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		AssetDatabase::AssetInfo info = AssetDatabase::ReadAssetInfo(importPath);

		if (asset && info.Filepath == filepath)
		{
			// If the sound already exists, just update some parameters and save
			Asset<Sound> sound(std::move(asset));

			sound->SetProps(m_Props);

			sound.Save(m_ImporterProps.Extension, filepath, importPath);
		}
		else
		{
			// If the sound doesn't exist, or the filepath has changed, reload everything
			Asset<Sound> sound = Sound::Create(filepath.string(), m_Props);

			sound.Save(m_ImporterProps.Extension, filepath, importPath);
		}
	}
#endif

	void SoundAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Sound* sound = static_cast<Sound*>(asset);

		const SoundProps& props = sound->GetProps();

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