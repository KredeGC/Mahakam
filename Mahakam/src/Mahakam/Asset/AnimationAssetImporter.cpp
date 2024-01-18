#include "Mahakam/mhpch.h"
#include "AnimationAssetImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Animation.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	AnimationAssetImporter::AnimationAssetImporter()
	{
		Setup(m_ImporterProps, "Animation", ".anim");
	}

#ifndef MH_STANDALONE
	void AnimationAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		if (node.valid() && node.has_child("Index"))
		{
			node["Index"] >> m_Index;
		}
	}

	void AnimationAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		GUI::DrawIntDrag("Animation Index", m_Index, 1, 0, 0);
	}

	void AnimationAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		
	}
#endif

	void AnimationAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Animation* animationAsset = static_cast<Animation*>(asset);

		node["Filepath"] << animationAsset->GetFilepath();
		node["Index"] << animationAsset->GetIndex();
	}

	Asset<void> AnimationAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		if (node.has_child("Filepath"))
		{
			std::string filepath;
			node["Filepath"] >> filepath;

			int index = 0;
			if (node.has_child("Index"))
				node["Index"] >> index;

			return Animation::Load(filepath);
		}

		return nullptr;
	}
}