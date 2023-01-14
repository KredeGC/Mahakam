#pragma once

#include "AssetImporter.h"

#include "Mahakam/Renderer/TextureProps.h"

namespace Mahakam
{
	class FrameBuffer;
	class Texture;

	class TextureAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

		std::filesystem::path m_Filepath;

		TextureProps m_Props2D;
		CubeTextureProps m_PropsCube;

		int m_TextureType = 0;

		Asset<Texture> m_Texture;
		Asset<FrameBuffer> m_PreviewBuffer;

	public:
		TextureAssetImporter();
		~TextureAssetImporter() = default;

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

#ifndef MH_STANDALONE
		virtual void OnWizardOpen(const std::filesystem::path& filepath, ryml::NodeRef& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;
        virtual bool OnIconRender(Asset<void> asset) override;
#endif

		virtual void Serialize(ryml::NodeRef& node, Asset<void> asset) override;
		virtual Asset<void> Deserialize(ryml::NodeRef& node) override;

	private:
		void CreateTexture(const std::filesystem::path& filepath);
	};
}