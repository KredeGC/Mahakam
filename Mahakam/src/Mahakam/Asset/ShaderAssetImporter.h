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
		virtual Ref<void> OnWizardImport(const std::filesystem::path& filepath) override;

		virtual void Serialize(YAML::Emitter& emitter, Ref<void> asset) override;
		virtual Ref<void> Deserialize(YAML::Node& node) override;
	};
}