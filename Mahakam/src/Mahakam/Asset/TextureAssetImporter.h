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

		TextureProps m_Props2D;
		CubeTextureProps m_PropsCube;

		int m_TextureType = 0;

		Ref<Texture> m_Texture;
		Ref<FrameBuffer> m_PreviewBuffer;

	public:
		TextureAssetImporter();
		~TextureAssetImporter() = default;

		virtual const ImporterProps& GetImporterProps() const override { return m_ImporterProps; }

#ifndef MH_STANDALONE
		virtual void OnWizardOpen(const std::filesystem::path& filepath, YAML::Node& node) override;
		virtual void OnWizardRender(const std::filesystem::path& filepath) override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;
#endif

		virtual void Serialize(YAML::Emitter& emitter, Ref<void> asset) override;
		virtual Ref<void> Deserialize(YAML::Node& node) override;

	private:
		void CreateTexture(const std::filesystem::path& filepath);
	};
}