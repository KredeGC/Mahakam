#include "Mahakam/mhpch.h"
#include "ShaderResourceImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/Shader.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

namespace Mahakam
{
	ShaderResourceImporter::ShaderResourceImporter() :
		ResourceImporter("Shader", ".shader", "shader"),
		m_Filepath()
	{
		m_ImporterProps.NoWizard = true;
	}

	void ShaderResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Filepath = filepath;
	}

	void ShaderResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		DeserializeYAMLNode(node, "Filepath", m_Filepath);
	}

	void ShaderResourceImporter::OnRender()
	{
		ImGui::Text("Shaders have no options");
	}

	void ShaderResourceImporter::OnImport(ryml::NodeRef& node)
	{
		SerializeYAMLNode(node, "Filepath", m_Filepath);
	}

	Asset<void> ShaderResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		if (filepath.empty())
			return nullptr;

		return Shader::Create(filepath);
	}
}