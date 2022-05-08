#include "mhpch.h"
#include "ShaderAssetImporter.h"

#include <imgui.h>

namespace Mahakam
{
	void ShaderAssetImporter::OnWizardOpen(YAML::Node& node)
	{
		
	}

	void ShaderAssetImporter::OnWizardRender()
	{
		ImGui::Text("Shaders have no options");
	}

	Ref<void> ShaderAssetImporter::OnWizardImport(const std::filesystem::path& filepath)
	{
		return Shader::Create(filepath.string());
	}

	void ShaderAssetImporter::Serialize(YAML::Emitter& emitter, Ref<void> asset)
	{
		//Ref<Shader> shader = StaticCastRef<Shader>(asset);
	}

	Ref<void> ShaderAssetImporter::Deserialize(YAML::Node& node)
	{
		YAML::Node filepathNode = node["Filepath"];
		std::string filepath;
		if (filepathNode)
			filepath = filepathNode.as<std::string>();

		return Shader::Create(filepath);
	}
}