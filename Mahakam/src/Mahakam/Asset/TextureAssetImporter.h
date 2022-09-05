#pragma once

#include "AssetImporter.h"

#include "Mahakam/Renderer/TextureProps.h"

namespace Mahakam
{
	class FrameBuffer;
	class Texture;

	extern template class Asset<FrameBuffer>;
	extern template class Asset<Texture>;

	class TextureAssetImporter : public AssetImporter
	{
	private:
		ImporterProps m_ImporterProps;

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
		virtual void OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;
#endif

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;

	private:
		void CreateTexture(const std::filesystem::path& filepath);
	};
}