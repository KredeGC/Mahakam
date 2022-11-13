#include "Mahakam/mhpch.h"
#include "AnimationAssetImporter.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	AnimationAssetImporter::AnimationAssetImporter()
	{
		m_ImporterProps.Extension = ".anim";
	}

#ifndef MH_STANDALONE
	void AnimationAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{

	}

	void AnimationAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::Text("No options yet");
	}

	void AnimationAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		
	}
#endif

	void AnimationAssetImporter::Serialize(ryml::NodeRef& node, Asset<void> asset)
	{
		// TODO: Properties
	}

	Asset<void> AnimationAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		return nullptr;
	}
}