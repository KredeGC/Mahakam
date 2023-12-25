#pragma once

#include "ResourceImporter.h"

#include "Mahakam/Renderer/TextureProps.h"

namespace Mahakam
{
	class Texture;

	class TextureCubeResourceImporter : public ResourceImporter
	{
	private:
		std::filesystem::path m_Filepath;

		CubeTextureProps m_Props;

		Asset<Texture> m_Texture;

	public:
		TextureCubeResourceImporter();

		virtual void OnResourceOpen(const std::filesystem::path& filepath) override;
		virtual void OnImportOpen(ryml::NodeRef& node) override;
		virtual void OnRender() override;
		virtual void OnImport(ryml::NodeRef& node) override;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) override;

	private:
		void CreatePreviewTexture();
		CubeTextureProps DeserializeProps(ryml::NodeRef& node);
	};
}