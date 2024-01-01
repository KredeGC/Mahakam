#include "Mahakam/mhpch.h"
#include "TextureAssetImporter.h"

#include "Mahakam/ImGui/GUI.h"

#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Texture.h"

#include "Mahakam/Serialization/YAMLSerialization.h"

#include <imgui/imgui.h>

#include <magic_enum/magic_enum.hpp>

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

	void TextureAssetImporter::Serialize(ryml::NodeRef& node, void* asset)
	{
		Texture* textureAsset = static_cast<Texture*>(asset);

		node["Filepath"] << textureAsset->GetFilepath();

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