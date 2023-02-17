#include "Mahakam/mhpch.h"
#include "TextureAssetImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Texture.h"

#include <imgui/imgui.h>

namespace Mahakam
{
	TextureAssetImporter::TextureAssetImporter()
	{
		Setup(m_ImporterProps, "Texture", ".texture");

		// Create preview buffer
		FrameBufferProps gProps;
		gProps.Width = 128;
		gProps.Height = 128;
		gProps.ColorAttachments = TrivialVector<FrameBufferAttachmentProps>{ TextureFormat::RGBA8 };

		m_PreviewBuffer = FrameBuffer::Create(gProps);
	}

#ifndef MH_STANDALONE
	void TextureAssetImporter::OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node)
	{
		m_Filepath = filepath;

		m_Props2D = TextureProps{};
		m_PropsCube = CubeTextureProps{};

		if (node.valid())
		{
			int enumInt;

			m_TextureType = 0;
			if (node.has_child("Type"))
				node["Type"] >> m_TextureType;

			if (node.has_child("Format"))
			{
				node["Format"] >> enumInt;
				if (m_TextureType == 0)
					m_Props2D.Format = (TextureFormat)enumInt;
				else
					m_PropsCube.Format = (TextureFormat)enumInt;
			}

			if (node.has_child("Filter"))
			{
				node["Filter"] >> enumInt;
				if (m_TextureType == 0)
					m_Props2D.FilterMode = (TextureFilter)enumInt;
				else
					m_PropsCube.FilterMode = (TextureFilter)enumInt;
			}

			if (m_TextureType == 0)
			{
				if (node.has_child("WrapX"))
				{
					node["WrapX"] >> enumInt;
					m_Props2D.WrapX = (TextureWrapMode)enumInt;
				}

				if (node.has_child("WrapY"))
				{
					node["WrapY"] >> enumInt;
					m_Props2D.WrapY = (TextureWrapMode)enumInt;
				}
			}
			else
			{
				if (node.has_child("Resolution"))
					node["Resolution"] >> m_PropsCube.Resolution;

				if (node.has_child("Prefilter"))
				{
					node["Prefilter"] >> enumInt;
					m_PropsCube.Prefilter = (TextureCubePrefilter)enumInt;
				}
			}

			if (node.has_child("Mipmaps"))
			{
				if (m_TextureType == 0)
					node["Mipmaps"] >> m_Props2D.Mipmaps;
				else
					node["Mipmaps"] >> m_PropsCube.Mipmaps;
			}
		}

		CreateTexture(filepath);
	}

	void TextureAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
	{
		if (filepath != m_Filepath && std::filesystem::exists(filepath))
		{
			m_Filepath = filepath;
			CreateTexture(filepath);
		}

		// Type (2D, Cube)
		const char* projectionTypeStrings[] = { "Texture2D", "TextureCube" };
		const char* currentTextureType = projectionTypeStrings[m_TextureType];

		if (ImGui::BeginCombo("Texture Type", currentTextureType))
		{
			for (int i = 0; i < 2; i++)
			{
				bool selected = currentTextureType == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], selected))
				{
					m_TextureType = i;
					CreateTexture(filepath);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// Format
		const char* textureFormatStrings[(int)TextureFormat::Count];

		for (int i = 0; i < (int)TextureFormat::Count; i++)
			textureFormatStrings[i] = GetStringTextureFormat((TextureFormat)i);

		const char* currentTextureFormat = textureFormatStrings[(int)(m_TextureType == 0 ? m_Props2D.Format : m_PropsCube.Format)];

		if (ImGui::BeginCombo("Texture Format", currentTextureFormat))
		{
			for (int i = 0; i < (int)TextureFormat::Count; i++)
			{
				bool selected = currentTextureFormat == textureFormatStrings[i];
				if (ImGui::Selectable(textureFormatStrings[i], selected))
				{
					if (m_TextureType == 0)
						m_Props2D.Format = (TextureFormat)i;
					else
						m_PropsCube.Format = (TextureFormat)i;
					CreateTexture(filepath);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// Filter
		const char* textureFilterStrings[(int)TextureFilter::Count];

		for (int i = 0; i < (int)TextureFilter::Count; i++)
			textureFilterStrings[i] = GetStringTextureFilter((TextureFilter)i);

		const char* currentTextureFilter = textureFilterStrings[(int)(m_TextureType == 0 ? m_Props2D.FilterMode : m_PropsCube.FilterMode)];

		if (ImGui::BeginCombo("Texture Filter", currentTextureFilter))
		{
			for (int i = 0; i < (int)TextureFilter::Count; i++)
			{
				bool selected = currentTextureFilter == textureFilterStrings[i];
				if (ImGui::Selectable(textureFilterStrings[i], selected))
				{
					if (m_TextureType == 0)
						m_Props2D.FilterMode = (TextureFilter)i;
					else
						m_PropsCube.FilterMode = (TextureFilter)i;
					CreateTexture(filepath);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// TextureWrap
		if (m_TextureType == 0)
		{
			const char* textureWrapStrings[(int)TextureWrapMode::Count];

			for (int i = 0; i < (int)TextureWrapMode::Count; i++)
				textureWrapStrings[i] = GetStringTextureWrapMode((TextureWrapMode)i);

			const char* currentWrapX = textureWrapStrings[(int)m_Props2D.WrapX];
			const char* currentWrapY = textureWrapStrings[(int)m_Props2D.WrapY];

			if (ImGui::BeginCombo("Texture Wrap X", currentWrapX))
			{
				for (int i = 0; i < (int)TextureWrapMode::Count; i++)
				{
					bool selected = currentWrapX == textureWrapStrings[i];
					if (ImGui::Selectable(textureWrapStrings[i], selected))
					{
						m_Props2D.WrapX = (TextureWrapMode)i;
						CreateTexture(filepath);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Texture Wrap Y", currentWrapY))
			{
				for (int i = 0; i < (int)TextureWrapMode::Count; i++)
				{
					bool selected = currentWrapY == textureWrapStrings[i];
					if (ImGui::Selectable(textureWrapStrings[i], selected))
					{
						m_Props2D.WrapY = (TextureWrapMode)i;
						CreateTexture(filepath);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
		else
		{
			int32_t resolution = (int32_t)m_PropsCube.Resolution;
			if (GUI::DrawIntDrag("Size", resolution, 32, 32, 8192))
				m_PropsCube.Resolution = (uint32_t)resolution;

			const char* prefilterStrings[(int)TextureCubePrefilter::Count];

			for (int i = 0; i < (int)TextureCubePrefilter::Count; i++)
				prefilterStrings[i] = GetStringTextureCubePrefilter((TextureCubePrefilter)i);

			const char* currentPrefilter = prefilterStrings[(int)m_PropsCube.Prefilter];

			if (ImGui::BeginCombo("Prefilter", currentPrefilter))
			{
				for (int i = 0; i < (int)TextureCubePrefilter::Count; i++)
				{
					bool selected = currentPrefilter == prefilterStrings[i];
					if (ImGui::Selectable(prefilterStrings[i], selected))
					{
						m_PropsCube.Prefilter = (TextureCubePrefilter)i;
						CreateTexture(filepath);
					}

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}

		// Mips
		bool* mipmaps = m_TextureType == 0 ? &m_Props2D.Mipmaps : &m_PropsCube.Mipmaps;
		if (ImGui::Checkbox("Mipmaps", mipmaps))
		{
			if (m_TextureType == 0)
				m_Props2D.Mipmaps = *mipmaps;
			else
				m_PropsCube.Mipmaps = *mipmaps;
			CreateTexture(filepath);
		}

		// Render preview texture
		if (m_Texture)
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			float maxSize = glm::min(size.x, size.y);
			ImVec2 viewportSize = { size.x, maxSize };

			ImGui::BeginChild("Texture Preview", viewportSize);

			if (m_TextureType == 0) // Render 2D texture
			{
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
			}
			else // Render cubemap texture
			{

			}

			ImGui::EndChild();
		}
	}

	void TextureAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		m_Texture.Save(m_ImporterProps.Extension, filepath, importPath);

		//AssetDatabase::ReloadAsset(m_Texture.GetID());

		m_Texture = nullptr;
	}
    
    bool TextureAssetImporter::OnIconRender(Asset<void> asset)
    {
        Asset<Texture> textureAsset(asset);
        
        if (!textureAsset->IsCubemap())
        {
			Asset<Texture2D> texture2D(textureAsset);
            
            texture2D->Bind();
            
            GL::DrawScreenQuad();
            
            return true;
        }
        
        return false;
    }
#endif

	void TextureAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Texture* textureAsset = static_cast<Texture*>(asset);

		if (!textureAsset->IsCubemap())
		{
			Texture2D* texture2D = static_cast<Texture2D*>(textureAsset);

			node["Type"] << 0;
			node["Format"] << (int)texture2D->GetProps().Format;
			node["Filter"] << (int)texture2D->GetProps().FilterMode;
			node["WrapX"] << (int)texture2D->GetProps().WrapX;
			node["WrapY"] << (int)texture2D->GetProps().WrapY;
			node["Mipmaps"] << texture2D->GetProps().Mipmaps;
		}
		else
		{
			TextureCube* textureCube = static_cast<TextureCube*>(textureAsset);

			node["Type"] << 1;
			node["Format"] << (int)textureCube->GetProps().Format;
			node["Filter"] << (int)textureCube->GetProps().FilterMode;
			node["Resolution"] << textureCube->GetProps().Resolution;
			node["Prefilter"] << (int)textureCube->GetProps().Prefilter;
			node["Mipmaps"] << textureCube->GetProps().Mipmaps;
		}
	}

	Asset<void> TextureAssetImporter::Deserialize(ryml::NodeRef& node)
	{
		TextureProps props2D;
		CubeTextureProps propsCube;

		int enumInt;

		std::string filepath;
		if (node.has_child("Filepath"))
			node["Filepath"] >> filepath;

		int textureType = 0;
		if (node.has_child("Type"))
			node["Type"] >> textureType;

		if (node.has_child("Format"))
		{
			node["Format"] >> enumInt;
			if (textureType == 0)
				props2D.Format = (TextureFormat)enumInt;
			else
				propsCube.Format = (TextureFormat)enumInt;
		}

		if (node.has_child("Filter"))
		{
			node["Filter"] >> enumInt;
			if (textureType == 0)
				props2D.FilterMode = (TextureFilter)enumInt;
			else
				propsCube.FilterMode = (TextureFilter)enumInt;
		}

		if (textureType == 0)
		{
			if (node.has_child("WrapX"))
			{
				node["WrapX"] >> enumInt;
				props2D.WrapX = (TextureWrapMode)enumInt;
			}

			if (node.has_child("WrapY"))
			{
				node["WrapY"] >> enumInt;
				props2D.WrapY = (TextureWrapMode)enumInt;
			}
		}
		else
		{
			if (node.has_child("Resolution"))
				node["Resolution"] >> propsCube.Resolution;

			if (node.has_child("Prefilter"))
			{
				node["Prefilter"] >> enumInt;
				propsCube.Prefilter = (TextureCubePrefilter)enumInt;
			}
		}

		if (node.has_child("Mipmaps"))
		{
			if (textureType == 0)
				node["Mipmaps"] >> props2D.Mipmaps;
			else
				node["Mipmaps"] >> propsCube.Mipmaps;
		}

		if (textureType == 0)
			return Texture2D::Create(filepath, props2D);
		else
			return TextureCube::Create(filepath, propsCube);

		return nullptr;
	}

	void TextureAssetImporter::CreateTexture(const std::filesystem::path& filepath)
	{
		if (m_TextureType == 0)
			m_Texture = Texture2D::Create(filepath.string(), m_Props2D);
		else
			m_Texture = TextureCube::Create(filepath.string(), m_PropsCube);
	}
}