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
	void ShaderAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node) {}

	void ShaderAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::Text("Shaders have no options");
	}

	void ShaderAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<Shader> shaderAsset = Shader::Create(filepath);

		shaderAsset.Save(filepath, importPath);
	}
#endif

	void ShaderAssetImporter::Serialize(ryml::NodeRef& node, Asset<void> asset)
	{
		//Ref<Shader> shader = StaticCastRef<Shader>(asset);
	}

	Asset<void> ShaderAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		if (node.has_child("Filepath"))
		{
			std::string filepath;
			node["Filepath"] >> filepath;

			return Shader::Create(filepath);
		}

		return nullptr;
	}
}