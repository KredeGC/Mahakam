#include "Mahakam/mhpch.h"
#include "ShaderAssetImporter.h"

#include "Mahakam/Renderer/Shader.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	ShaderAssetImporter::ShaderAssetImporter()
	{
		Setup(m_ImporterProps, "Shader", ".shader");

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

		shaderAsset.Save(m_ImporterProps.Extension, importPath);
	}
#endif

	void ShaderAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Shader* shader = static_cast<Shader*>(asset);

		SerializeYAMLNode(node, "Filepath", shader->GetFilepath());
	}

	Asset<void> ShaderAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		if (node.has_child("Filepath"))
		{
			std::filesystem::path filepath;
			node["Filepath"] >> filepath;

			return Shader::Create(filepath);
		}

		return nullptr;
	}
}