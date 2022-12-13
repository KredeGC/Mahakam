#include "Mahakam/mhpch.h"
#include "SoundAssetImporter.h"

#include "Mahakam/Audio/Sound.h"

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
		m_Props.Volume = 1.0f;
		if (node.has_child("Volume"))
			node["Volume"] >> m_Props.Volume;

		m_Props.Loop = false;
		if (node.has_child("Loop"))
			node["Loop"] >> m_Props.Loop;
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

			AssetDatabase::ReloadAsset(sound.GetID());
		}
	}
#endif

	void SoundAssetImporter::Serialize(ryml::NodeRef& node, Asset<void> asset)
	{
		Asset<Sound> sound(asset);

		node["Volume"] << sound->GetProps().Volume;
		node["Loop"] << sound->GetProps().Loop;
	}

	Asset<void> SoundAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		std::string filepath;
		if (node.has_child("Filepath"))
			node["Filepath"] >> filepath;

		float volume;
		if (node.has_child("Volume"))
			node["Volume"] >> volume;

		bool loop = false;
		if (node.has_child("Loop"))
			node["Loop"] >> loop;

		return Sound::Create(filepath, { volume, loop });
	}
}