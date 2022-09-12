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
	void AnimationAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node)
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

	void AnimationAssetImporter::Serialize(YAML::Emitter& emitter, Ref<void> asset)
	{
		// TODO: Properties
	}

	Ref<void> AnimationAssetImporter::Deserialize(YAML::Node& node)
	{
		return nullptr;
	}
}