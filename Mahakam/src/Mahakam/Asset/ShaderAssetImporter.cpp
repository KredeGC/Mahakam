#include "Mahakam/mhpch.h"
#include "ShaderAssetImporter.h"

#include "Mahakam/Renderer/Shader.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	ShaderAssetImporter::ShaderAssetImporter()
	{
		m_ImporterProps.Extension = ".shader";
		m_ImporterProps.NoWizard = true;
	}

#ifndef MH_STANDALONE
	void ShaderAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) { }

	void ShaderAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::Text("Shaders have no options");
	}

	void ShaderAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Shader> shaderAsset = Shader::Create(filepath.string());

		shaderAsset.Save(filepath, importPath);

		AssetDatabase::ReloadAsset(shaderAsset.GetID());
	}
#endif

	void ShaderAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		//Ref<Shader> shader = StaticCastRef<Shader>(asset);
	}

	Asset<void> ShaderAssetImporter::Deserialize(YAML::Node& node)
	{
		YAML::Node filepathNode = node["Filepath"];
		if (filepathNode)
		{
			std::string filepath = filepathNode.as<std::string>();

			return Shader::Create(filepath);
		}

		return nullptr;
	}
}