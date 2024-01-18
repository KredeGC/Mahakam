#pragma once

#include "ResourceImporter.h"

#include "Mahakam/Audio/SoundProps.h"

namespace Mahakam
{
	class SoundResourceImporter : public ResourceImporter
	{
	private:
		std::filesystem::path m_Filepath;
		SoundProps m_Props;

	public:
		SoundResourceImporter();

		virtual void OnResourceOpen(const std::filesystem::path& filepath) override;
		virtual void OnImportOpen(ryml::NodeRef& node) override;
		virtual void OnRender() override;
		virtual void OnImport(ryml::NodeRef& node) override;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) override;

	private:
		SoundProps DeserializeProps(ryml::NodeRef& node);
	};
}