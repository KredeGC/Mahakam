#pragma once

#include "Mahakam/Core/Core.h"

#include <filesystem>
#include <string>

#include <yaml-cpp/yaml.h>

namespace Mahakam
{
	class AssetImporter
	{
	public:
		virtual void OnWizardOpen(YAML::Node& node) = 0;
		virtual void OnWizardRender() = 0;
		virtual Ref<void> OnWizardImport(const std::filesystem::path& filepath) = 0;

		virtual void Serialize(YAML::Emitter& emitter, Ref<void> asset) = 0;
		virtual Ref<void> Deserialize(YAML::Node& node) = 0;
	};
}