#include "mhpch.h"
#include "TextureAssetImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include <imgui.h>

namespace Mahakam
{
	TextureAssetImporter::TextureAssetImporter()
	{
		m_ImporterProps.Extension = ".texture";
	}

	void TextureAssetImporter::OnWizardOpen(YAML::Node& node)
	{
		m_Props2D = TextureProps{};
		m_PropsCube = CubeTextureProps{};

		YAML::Node cubeNode = node["Type"];
		m_TextureType = 0;
		if (cubeNode)
			m_TextureType = cubeNode.as<int>();

		YAML::Node formatNode = node["Format"];
		if (formatNode)
		{
			if (m_TextureType == 0)
				m_Props2D.format = (TextureFormat)formatNode.as<int>();
			else
				m_PropsCube.format = (TextureFormat)formatNode.as<int>();
		}

		YAML::Node filterNode = node["Filter"];
		if (filterNode)
		{
			if (m_TextureType == 0)
				m_Props2D.filterMode = (TextureFilter)filterNode.as<int>();
			else
				m_PropsCube.filterMode = (TextureFilter)filterNode.as<int>();
		}

		if (m_TextureType == 0)
		{
			YAML::Node wrapXNode = node["WrapX"];
			if (wrapXNode)
				m_Props2D.wrapX = (TextureWrapMode)wrapXNode.as<int>();

			YAML::Node wrapYNode = node["WrapY"];
			if (wrapYNode)
				m_Props2D.wrapY = (TextureWrapMode)wrapYNode.as<int>();
		}
		else
		{
			YAML::Node resolutionNode = node["Resolution"];
			if (resolutionNode)
				m_PropsCube.resolution = resolutionNode.as<uint32_t>();

			YAML::Node prefilterNode = node["Prefilter"];
			if (prefilterNode)
				m_PropsCube.prefilter = (TextureCubePrefilter)prefilterNode.as<int>();
		}

		YAML::Node mipmapsNode = node["Mipmaps"];
		if (mipmapsNode)
		{
			if (m_TextureType == 0)
				m_Props2D.mipmaps = mipmapsNode.as<bool>();
			else
				m_PropsCube.mipmaps = mipmapsNode.as<bool>();
		}
	}

	void TextureAssetImporter::OnWizardRender(const std::filesystem::path& filepath)
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
		else
		{
			int32_t resolution = (int32_t)m_PropsCube.resolution;
			if (GUI::DrawIntDrag("Size", resolution, 32, 32, 8192))
				m_PropsCube.resolution = (uint32_t)resolution;

			const char* prefilterStrings[(int)TextureCubePrefilter::Count];

			for (int i = 0; i < (int)TextureCubePrefilter::Count; i++)
				prefilterStrings[i] = GetStringTextureCubePrefilter((TextureCubePrefilter)i);

			const char* currentPrefilter = prefilterStrings[(int)m_PropsCube.prefilter];

			if (ImGui::BeginCombo("Prefilter", currentPrefilter))
			{
				for (int i = 0; i < (int)TextureCubePrefilter::Count; i++)
				{
					bool selected = currentPrefilter == prefilterStrings[i];
					if (ImGui::Selectable(prefilterStrings[i], selected))
						m_PropsCube.prefilter = (TextureCubePrefilter)i;

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

	void TextureAssetImporter::OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath)
	{
		if (m_TextureType == 0)
		{
			Asset<Texture2D> texture = Texture2D::Create(filepath.string(), m_Props2D);

			texture.Save(filepath, importPath);

			AssetDatabase::ReloadAsset(texture.GetID());
		}
		else
		{
			Asset<TextureCube> texture = TextureCube::Create(filepath.string(), m_PropsCube);

			texture.Save(filepath, importPath);

			AssetDatabase::ReloadAsset(texture.GetID());
		}
	}

	void TextureAssetImporter::Serialize(YAML::Emitter& emitter, Asset<void> asset)
	{
		Asset<Texture> texture(asset);

		if (!texture->IsCubemap())
		{
			Asset<Texture2D> texture2D(texture);

			emitter << YAML::Key << "Type";
			emitter << YAML::Value << 0;
			emitter << YAML::Key << "Format";
			emitter << YAML::Value << (int)texture2D->GetProps().format;
			emitter << YAML::Key << "Filter";
			emitter << YAML::Value << (int)texture2D->GetProps().filterMode;
			emitter << YAML::Key << "WrapX";
			emitter << YAML::Value << (int)texture2D->GetProps().wrapX;
			emitter << YAML::Key << "WrapY";
			emitter << YAML::Value << (int)texture2D->GetProps().wrapY;
			emitter << YAML::Key << "Mipmaps";
			emitter << YAML::Value << texture2D->GetProps().mipmaps;
		}
		else
		{
			Asset<TextureCube> textureCube(texture);

			emitter << YAML::Key << "Type";
			emitter << YAML::Value << 1;
			emitter << YAML::Key << "Format";
			emitter << YAML::Value << (int)textureCube->GetProps().format;
			emitter << YAML::Key << "Filter";
			emitter << YAML::Value << (int)textureCube->GetProps().filterMode;
			emitter << YAML::Key << "Resolution";
			emitter << YAML::Value << textureCube->GetProps().resolution;
			emitter << YAML::Key << "Prefilter";
			emitter << YAML::Value << (int)textureCube->GetProps().prefilter;
			emitter << YAML::Key << "Mipmaps";
			emitter << YAML::Value << textureCube->GetProps().mipmaps;
		}
	}

	Asset<void> TextureAssetImporter::Deserialize(YAML::Node& node)
	{
		TextureProps props2D;
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
		else
		{
			YAML::Node resolutionNode = node["Resolution"];
			if (resolutionNode)
				propsCube.resolution = resolutionNode.as<uint32_t>();

			YAML::Node prefilterNode = node["Prefilter"];
			if (prefilterNode)
				propsCube.prefilter = (TextureCubePrefilter)prefilterNode.as<int>();
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
			return TextureCube::Create(filepath, propsCube);

		return nullptr;
	}
}