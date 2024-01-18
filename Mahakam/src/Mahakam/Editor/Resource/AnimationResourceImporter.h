#pragma once

#include "ResourceImporter.h"

namespace Mahakam
{
	class AnimationResourceImporter : public ResourceImporter
	{
	private:
		std::filesystem::path m_Filepath;
		int m_Index;

	public:
		AnimationResourceImporter();

		virtual void OnResourceOpen(const std::filesystem::path& filepath) override;
		virtual void OnImportOpen(ryml::NodeRef& node) override;
		virtual void OnRender() override;
		virtual void OnImport(ryml::NodeRef& node) override;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) override;
	};
}