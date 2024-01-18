#pragma once

#include "ResourceImporter.h"

#include "Mahakam/Renderer/TextureProps.h"

namespace Mahakam
{
	class Texture;

	class Texture2DResourceImporter : public ResourceImporter
	{
	private:
		std::filesystem::path m_Filepath;

		TextureProps m_Props;

		Asset<Texture> m_Texture;

	public:
		Texture2DResourceImporter();

		virtual void OnResourceOpen(const std::filesystem::path& filepath) override;
		virtual void OnImportOpen(ryml::NodeRef& node) override;
		virtual void OnRender() override;
		virtual void OnImport(ryml::NodeRef& node) override;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) override;

	private:
		void CreatePreviewTexture();
		TextureProps DeserializeProps(ryml::NodeRef& node);
	};
}