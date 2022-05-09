#include "mhpch.h"
#include "TextureAssetImporter.h"

#include <imgui.h>

namespace Mahakam
{
	void TextureAssetImporter::OnWizardOpen(YAML::Node& node)
	{
		m_Props2D = TextureProps{};
		m_PropsCube = CubeTextureProps{};

		YAML::Node cubeNode = node["Type"];
		m_TextureType = 0;
		if (cubeNode)
			m_TextureType = cubeNode.as<int>();
	}

	void TextureAssetImporter::OnWizardRender()
	{
		// Type (2D, Cube)
		const char* projectionTypeStrings[] = { "Texture2D", "TextureCube" };
		const char* currentTextureType = projectionTypeStrings[m_TextureType];

		if (ImGui::BeginCombo("Texture Type", currentTextureType))
		{
			for (int i = 0; i < 2; i++)
			{
				bool selected = currentTextureType == projectionTypeStrings[i];
				if (ImGui::Selectable(projectionTypeStrings[i], selected))
					m_TextureType = i;

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// Format
		const char* textureFormatStrings[(int)TextureFormat::Count];

		for (int i = 0; i < (int)TextureFormat::Count; i++)
			textureFormatStrings[i] = GetStringTextureFormat((TextureFormat)i);

		const char* currentTextureFormat = textureFormatStrings[(int)(m_TextureType == 0 ? m_Props2D.format : m_PropsCube.format)];

		if (ImGui::BeginCombo("Texture Format", currentTextureFormat))
		{
			for (int i = 0; i < (int)TextureFormat::Count; i++)
			{
				bool selected = currentTextureFormat == textureFormatStrings[i];
				if (ImGui::Selectable(textureFormatStrings[i], selected))
				{
					if (m_TextureType == 0)
						m_Props2D.format = (TextureFormat)i;
					else
						m_PropsCube.format = (TextureFormat)i;
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

		const char* currentTextureFilter = textureFilterStrings[(int)(m_TextureType == 0 ? m_Props2D.filterMode : m_PropsCube.filterMode)];

		if (ImGui::BeginCombo("Texture Filter", currentTextureFilter))
		{
			for (int i = 0; i < (int)TextureFilter::Count; i++)
			{
				bool selected = currentTextureFilter == textureFilterStrings[i];
				if (ImGui::Selectable(textureFilterStrings[i], selected))
				{
					if (m_TextureType == 0)
						m_Props2D.filterMode = (TextureFilter)i;
					else
						m_PropsCube.filterMode = (TextureFilter)i;
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

			const char* currentWrapX = textureWrapStrings[(int)m_Props2D.wrapX];
			const char* currentWrapY = textureWrapStrings[(int)m_Props2D.wrapY];

			if (ImGui::BeginCombo("Texture Wrap X", currentWrapX))
			{
				for (int i = 0; i < (int)TextureWrapMode::Count; i++)
				{
					bool selected = currentWrapX == textureWrapStrings[i];
					if (ImGui::Selectable(textureWrapStrings[i], selected))
						m_Props2D.wrapX = (TextureWrapMode)i;

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
						m_Props2D.wrapY = (TextureWrapMode)i;

					if (selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}

		// Mips
		bool* mipmaps = m_TextureType == 0 ? &m_Props2D.mipmaps : &m_PropsCube.mipmaps;
		if (ImGui::Checkbox("Mipmaps", mipmaps))
		{
			if (m_TextureType == 0)
				m_Props2D.mipmaps = *mipmaps;
			else
				m_PropsCube.mipmaps = *mipmaps;
		}
	}

	Asset<void> TextureAssetImporter::OnWizardImport(const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		if (m_TextureType == 0)
			return Texture2D::Create(filepath.string(), m_Props2D);
		else
			return TextureCube::Create(filepath.string(), m_PropsCube);
	}

	void TextureAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		/*Asset<Texture> textureAsset = StaticCastRef<Texture>(asset);
		Asset<Texture2D> texture2D = std::dynamic_pointer_cast<Texture2D>(textureAsset);
		Asset<TextureCube> textureCube = std::dynamic_pointer_cast<TextureCube>(textureAsset);

		emitter << YAML::Key << "Type";
		if (texture2D)
			emitter << YAML::Value << 0;
		else
			emitter << YAML::Value << 1;

		emitter << YAML::Key << "Format";
		if (texture2D)
			emitter << YAML::Value << (int)texture2D->GetProps().format;
		else
			emitter << YAML::Value << (int)textureCube->GetProps().format;

		emitter << YAML::Key << "Filter";
		if (texture2D)
			emitter << YAML::Value << (int)texture2D->GetProps().filterMode;
		else
			emitter << YAML::Value << (int)textureCube->GetProps().filterMode;

		if (texture2D)
		{
			emitter << YAML::Key << "WrapX";
			emitter << YAML::Value << (int)texture2D->GetProps().wrapX;

			emitter << YAML::Key << "WrapY";
			emitter << YAML::Value << (int)texture2D->GetProps().wrapY;
		}

		emitter << YAML::Key << "Mipmaps";
		if (texture2D)
			emitter << YAML::Value << texture2D->GetProps().mipmaps;
		else
			emitter << YAML::Value << textureCube->GetProps().mipmaps;*/
	}

	Asset<void> TextureAssetImporter::Deserialize(YAML::Node& node)
	{
		/*TextureProps props2D;
		CubeTextureProps propsCube;

		YAML::Node filepathNode = node["Filepath"];
		std::string filepath;
		if (filepathNode)
			filepath = filepathNode.as<std::string>();

		YAML::Node typeNode = node["Type"];
		int textureType = 0;
		if (typeNode)
			textureType = typeNode.as<int>();

		YAML::Node formatNode = node["Format"];
		if (formatNode)
		{
			if (textureType == 0)
				props2D.format = (TextureFormat)formatNode.as<int>();
			else
				propsCube.format = (TextureFormat)formatNode.as<int>();
		}

		YAML::Node filterNode = node["Filter"];
		if (filterNode)
		{
			if (textureType == 0)
				props2D.filterMode = (TextureFilter)filterNode.as<int>();
			else
				propsCube.filterMode = (TextureFilter)filterNode.as<int>();
		}

		if (textureType == 0)
		{
			YAML::Node wrapXNode = node["WrapX"];
			if (wrapXNode)
					props2D.wrapX = (TextureWrapMode)wrapXNode.as<int>();

			YAML::Node wrapYNode = node["WrapY"];
			if (wrapYNode)
				props2D.wrapY = (TextureWrapMode)wrapYNode.as<int>();
		}

		YAML::Node mipmapsNode = node["Mipmaps"];
		if (mipmapsNode)
		{
			if (textureType == 0)
				props2D.mipmaps = mipmapsNode.as<bool>();
			else
				propsCube.mipmaps = mipmapsNode.as<bool>();
		}

		if (textureType == 0)
			return Texture2D::Create(filepath, props2D);
		else
			return TextureCube::Create(filepath, propsCube);*/

		return nullptr;
	}
}