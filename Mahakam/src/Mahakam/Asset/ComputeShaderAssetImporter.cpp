#include "Mahakam/mhpch.h"
#include "ComputeShaderAssetImporter.h"

#include "Mahakam/Renderer/ComputeShader.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	ComputeShaderAssetImporter::ComputeShaderAssetImporter()
	{
		Setup(m_ImporterProps, "Compute Shader", ".compute");

		m_ImporterProps.NoWizard = true;
	}

#ifndef MH_STANDALONE
	void ComputeShaderAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node) { }

	void ComputeShaderAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::Text("Compute shaders have no options");
	}

	void ComputeShaderAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<ComputeShader> shaderAsset = ComputeShader::Create(filepath.string());

		shaderAsset.Save(m_ImporterProps.Extension, filepath, importPath);
	}
#endif

	void ComputeShaderAssetImporter::Serialize(ryml::NodeRef& node, Asset<void> asset)
	{
		//Ref<Shader> shader = StaticCastRef<Shader>(asset);
	}

	Asset<void> ComputeShaderAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		if (node.has_child("Filepath"))
		{
			std::string filepath;
			node["Filepath"] >> filepath;

			return ComputeShader::Create(filepath);
		}

		return nullptr;
	}
}