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

	Asset<void> ShaderAssetImporter::OnWizardImport(const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		return Shader::Create(filepath.string());
	}

	void ShaderAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		//Ref<Shader> shader = StaticCastRef<Shader>(asset);
	}

	Asset<void> ShaderAssetImporter::Deserialize(YAML::Node& node)
	{
		YAML::Node filepathNode = node["Filepath"];
		std::string filepath;
		if (filepathNode)
			filepath = filepathNode.as<std::string>();

		return Shader::Create(filepath);
	}
}