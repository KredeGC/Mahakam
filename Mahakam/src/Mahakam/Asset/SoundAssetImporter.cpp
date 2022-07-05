#include "mhpch.h"
#include "SoundAssetImporter.h"

#include "Mahakam/Audio/Sound.h"

#include <imgui.h>

namespace Mahakam
{
	SoundAssetImporter::SoundAssetImporter()
	{
		m_ImporterProps.Extension = ".sound";
	}

	void SoundAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node)
	{
		YAML::Node volumeNode = node["Volume"];
		m_Props.volume = 1.0f;
		if (volumeNode)
			m_Props.volume = volumeNode.as<float>();

		YAML::Node loopNode = node["Loop"];
		m_Props.loop = false;
		if (loopNode)
			m_Props.loop = loopNode.as<bool>();
	}

	void SoundAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::DragFloat("Sound Volume", &m_Props.volume, 0.01f, 0.0f);
		ImGui::Checkbox("Sound Looping", &m_Props.loop);
	}

	void SoundAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		AssetDatabase::AssetInfo info = AssetDatabase::ReadAssetInfo(importPath);

		if (asset && info.Filepath == filepath)
		{
			// If the sound already exists, just update some parameters and save
			Asset<Sound> sound(asset);

			sound->SetProps(m_Props);

			sound.Save(filepath, importPath);
		}
		else
		{
			// If the sound doesn't exist, or the filepath has changed, reload everything
			Asset<Sound> sound = Sound::Create(filepath.string(), m_Props);

			sound.Save(filepath, importPath);

			AssetDatabase::ReloadAsset(sound.GetID());
		}
	}

	void SoundAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		Asset<Sound> sound = asset;

		emitter << YAML::Key << "Volume";
		emitter << YAML::Value << sound->GetProps().volume;
		emitter << YAML::Key << "Loop";
		emitter << YAML::Value << sound->GetProps().loop;
	}

	Asset<void> SoundAssetImporter::Deserialize(YAML::Node& node)
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