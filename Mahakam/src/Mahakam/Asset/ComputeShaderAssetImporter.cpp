#include "Mahakam/mhpch.h"
#include "ComputeShaderAssetImporter.h"

#include "Mahakam/Renderer/ComputeShader.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	ComputeShaderAssetImporter::ComputeShaderAssetImporter()
	{
		Setup(m_ImporterProps, "Compute Shader", ".compute");

		m_ImporterProps.NoWizard = true;
	}

	void ComputeShaderAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		ComputeShader* shader = static_cast<ComputeShader*>(asset);

		// TODO
		//node["Filepath"] << shader->GetFilepath();
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