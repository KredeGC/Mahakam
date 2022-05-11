#pragma once

#include "AssetImporter.h"

#include "Mahakam/Renderer/Shader.h"

namespace Mahakam
{
	class ShaderAssetImporter : public AssetImporter
	{
	public:
		virtual void OnWizardOpen(YAML::Node& node) override;
		virtual void OnWizardRender() override;
		virtual void OnWizardImport(Asset<void> asset, const std::filesystem::path& filepath, const std::filesystem::path& importPath) override;

		virtual void Serialize(YAML::Emitter& emitter, Asset<void> asset) override;
		virtual Asset<void> Deserialize(YAML::Node& node) override;
	};
}