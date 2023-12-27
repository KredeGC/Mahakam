#include "Mahakam/mhpch.h"
#include "Texture2DResourceImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Texture.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

#include <magic_enum/magic_enum.hpp>

namespace Mahakam
{
	Texture2DResourceImporter::Texture2DResourceImporter() :
		ResourceImporter("Texture2D", ".tex2d"),
		m_Filepath(),
		m_Props(),
		m_Texture() {}

	void Texture2DResourceImporter::OnResourceOpen(const std::filesystem::path& filepath)
	{
		m_Filepath = filepath;
		m_Props = TextureProps();

		CreatePreviewTexture();
	}

	void Texture2DResourceImporter::OnImportOpen(ryml::NodeRef& node)
	{
		DeserializeYAMLNode(node, "Filepath", m_Filepath);

		m_Props = DeserializeProps(node);

		CreatePreviewTexture();
	}

	void Texture2DResourceImporter::OnRender()
	{
		if (GUI::DrawDragDropField("File path", ".png", m_Filepath))
			CreatePreviewTexture();

		// Format
		if (GUI::DrawComboBox("Texture Format", m_Props.Format, magic_enum::enum_names<TextureFormat>()))
			CreatePreviewTexture();

		// Filter
		if (GUI::DrawComboBox("Texture Filter", m_Props.FilterMode, magic_enum::enum_names<TextureFilter>()))
			CreatePreviewTexture();

		// Texture Wrap X
		if (GUI::DrawComboBox("Texture Wrap X", m_Props.WrapX, magic_enum::enum_names<TextureWrapMode>()))
			CreatePreviewTexture();

		// Texture Wrap Y
		if (GUI::DrawComboBox("Texture Wrap Y", m_Props.WrapY, magic_enum::enum_names<TextureWrapMode>()))
			CreatePreviewTexture();

		// Mips
		if (ImGui::Checkbox("Mipmaps", &m_Props.Mipmaps))
			CreatePreviewTexture();

		// Render preview texture
		if (m_Texture)
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			float maxSize = glm::min(size.x, size.y);
			ImVec2 viewportSize = { size.x, maxSize };

			ImGui::BeginChild("Texture2D Preview", viewportSize);

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImGui::Image((void*)(uintptr_t)m_Texture->GetRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

			if (ImGui::IsItemHovered())
			{
				ImGuiIO& io = ImGui::GetIO();
				float my_tex_w = (float)viewportSize.x;
				float my_tex_h = (float)viewportSize.y;

				ImGui::BeginTooltip();
				float region_sz = 32.0f;
				float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
				float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
				float zoom = 4.0f;

				if (region_x < 0.0f)
					region_x = 0.0f;
				else if (region_x > my_tex_w - region_sz)
					region_x = my_tex_w - region_sz;

				if (region_y < 0.0f)
					region_y = 0.0f;
				else if (region_y > my_tex_h - region_sz)
					region_y = my_tex_h - region_sz;

				ImGui::Text("UV: (%.3f, %.3f)", (region_x + region_sz * 0.5f) / my_tex_w, (region_y + region_sz * 0.5f) / my_tex_h);
				ImVec2 uv0 = ImVec2(region_x / my_tex_w, my_tex_h - region_y / my_tex_h);
				ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, my_tex_h - (region_y + region_sz) / my_tex_h);
				ImGui::Image((void*)(uintptr_t)m_Texture->GetRendererID(), ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
				ImGui::EndTooltip();
			}

			ImGui::EndChild();
		}
	}

	void Texture2DResourceImporter::OnImport(ryml::NodeRef& node)
	{
		SerializeYAMLNode(node, "Filepath", m_Filepath);
		SerializeYAMLNode(node, "Format", m_Props.Format);
		SerializeYAMLNode(node, "Filter", m_Props.FilterMode);
		SerializeYAMLNode(node, "WrapX", m_Props.WrapX);
		SerializeYAMLNode(node, "WrapY", m_Props.WrapY);
		SerializeYAMLNode(node, "Mipmaps", m_Props.Mipmaps);
	}

	Asset<void> Texture2DResourceImporter::CreateAsset(ryml::NodeRef& node)
	{
		std::filesystem::path filepath;
		DeserializeYAMLNode(node, "Filepath", filepath);

		TextureProps props = DeserializeProps(node);

		return Texture2D::Create(filepath, props);
	}

	TextureProps Texture2DResourceImporter::DeserializeProps(ryml::NodeRef& node)
	{
		TextureProps props;

		DeserializeYAMLNode(node, "Format", props.Format);
		DeserializeYAMLNode(node, "Filter", props.FilterMode);

		DeserializeYAMLNode(node, "WrapX", props.WrapX);
		DeserializeYAMLNode(node, "WrapY", props.WrapY);

		DeserializeYAMLNode(node, "Mipmaps", props.Mipmaps);

		return props;
	}

	void Texture2DResourceImporter::CreatePreviewTexture()
	{
		m_Texture = Texture2D::Create(m_Filepath, m_Props);
	}
}