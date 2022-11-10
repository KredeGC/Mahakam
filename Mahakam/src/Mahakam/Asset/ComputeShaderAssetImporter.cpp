#include "Mahakam/mhpch.h"
#include "ComputeShaderAssetImporter.h"

#include "Mahakam/Renderer/ComputeShader.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	ComputeShaderAssetImporter::ComputeShaderAssetImporter()
	{
		m_ImporterProps.Extension = ".compute";
		m_ImporterProps.NoWizard = true;
	}

#ifndef MH_STANDALONE
	void ComputeShaderAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) { }

	void ComputeShaderAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		ImGui::Text("Compute shaders have no options");
	}

	void ComputeShaderAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		Asset<ComputeShader> shaderAsset = ComputeShader::Create(filepath.string());

		shaderAsset.Save(filepath, importPath);

		AssetDatabase::ReloadAsset(shaderAsset.GetID());
	}
#endif

	void ComputeShaderAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		//Ref<Shader> shader = StaticCastRef<Shader>(asset);
	}

	Asset<void> ComputeShaderAssetImporter::Deserialize(YAML::Node& node)
	{
		YAML::Node filepathNode = node["Filepath"];
		if (filepathNode)
		{
			std::string filepath = filepathNode.as<std::string>();

			return ComputeShader::Create(filepath);
		}

		return nullptr;
	}
}