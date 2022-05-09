#pragma once

#include "AssetImporter.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	class TextureAssetImporter : public AssetImporter
	{
	private:
		TextureProps m_Props2D;
		CubeTextureProps m_PropsCube;

		int m_TextureType = 0;

	public:
		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender() override;
		virtual void OnWizardImport(Ref<void>& asset, const std::filesystem::path& filepath) override;

		virtual void Serialize(YAML::Emitter& emitter, Ref<void> asset) override;
		virtual Ref<void> Deserialize(YAML::Node& node) override;
	};
}