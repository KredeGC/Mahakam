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