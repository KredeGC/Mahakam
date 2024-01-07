#include "Mahakam/mhpch.h"
#include "TextureCubeResourceImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Texture.h"
#include "Mahakam/Renderer/TextureUtility.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

#include <magic_enum/magic_enum.hpp>

namespace Mahakam
{
	TextureCubeResourceImporter::TextureCubeResourceImporter() :
		ResourceImporter("TextureCube", ".texcube"),
		m_Filepath(),
		m_Props(),
		m_Texture() {}

	void TextureCubeResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Filepath = filepath;
		m_Props = CubeTextureProps();

		CreatePreviewTexture();
	}

	void TextureCubeResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		DeserializeYAMLNode(node, "Filepath", m_Filepath);

		m_Props = DeserializeProps(node);

		m_Texture = nullptr;
	}

	void TextureCubeResourceImporter::OnRender()
	{
		if (GUI::DrawDragDropField("File path", ".png", m_Filepath))
			CreatePreviewTexture();

		// Format
		if (GUI::DrawComboBox("Texture Format", m_Props.Format, magic_enum::enum_names<TextureFormat>()))
			CreatePreviewTexture();

		// Filter
		if (GUI::DrawComboBox("Texture Filter", m_Props.FilterMode, magic_enum::enum_names<TextureFilter>()))
			CreatePreviewTexture();

		// Resolution
		int32_t resolution = (int32_t)m_Props.Resolution;
		if (GUI::DrawIntDrag("Size", resolution, 32, 32, 8192))
		{
			m_Props.Resolution = (uint32_t)resolution;
			CreatePreviewTexture();
		}

		// Prefilter
		if (GUI::DrawComboBox("Prefilter", m_Props.Prefilter, magic_enum::enum_names<TextureCubePrefilter>()))
			CreatePreviewTexture();

		// Mips
		if (ImGui::Checkbox("Mipmaps", &m_Props.Mipmaps))
			CreatePreviewTexture();

		// TODO: Render preview texture
	}

	void TextureCubeResourceImporter::OnImport(ryml::NodeRef& node)
	{
		SerializeYAMLNode(node, "Filepath", m_Filepath);
		SerializeYAMLNode(node, "Format", m_Props.Format);
		SerializeYAMLNode(node, "Filter", m_Props.FilterMode);
		SerializeYAMLNode(node, "Resolution", m_Props.Resolution);
		SerializeYAMLNode(node, "Prefilter", m_Props.Prefilter);
		SerializeYAMLNode(node, "Mipmaps", m_Props.Mipmaps);
	}

	Asset<void> TextureCubeResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		CubeTextureProps props = DeserializeProps(node);

		// Read image
		int w, h, channels;
		bool hdr;
		auto data = LoadImageFile(m_Filepath.string().c_str(), w, h, channels, hdr);

		// Convert image
		if (m_Props.Format == TextureFormat::RG11B10F && channels == 3)
		{
			auto pixels = ProjectEquirectangularToCubemap(reinterpret_cast<float*>(data.get()), w, h, channels, hdr, m_Props.Resolution);
			glm::vec3* buffer = reinterpret_cast<glm::vec3*>(pixels.data());

			size_t size = static_cast<size_t>(m_Props.Resolution) * m_Props.Resolution * 6ull;
			TrivialArray<uint32_t> converted_pixels(size);
			for (size_t i = 0; i < size; i++)
				converted_pixels[i] = Vec3ToRG11B10F(buffer[i]);
		}

		return TextureCube::Create(filepath, props);
	}

	CubeTextureProps TextureCubeResourceImporter::DeserializeProps(ryml::NodeRef& node)
	{
		CubeTextureProps props;

		DeserializeYAMLNode(node, "Format", props.Format);
		DeserializeYAMLNode(node, "Filter", props.FilterMode);
		DeserializeYAMLNode(node, "Resolution", props.Resolution);
		DeserializeYAMLNode(node, "Prefilter", props.Prefilter);
		DeserializeYAMLNode(node, "Mipmaps", props.Mipmaps);

		return props;
	}

	void TextureCubeResourceImporter::CreatePreviewTexture()
	{
		//m_Texture = Texture2D::Create(m_Filepath, m_Props);
	}
}