#pragma once

#include "ResourceImporter.h"

#include "Mahakam/Renderer/MeshProps.h"

namespace Mahakam
{
	class Mesh;

	class BoneMeshResourceImporter : public ResourceImporter
	{
	private:
		BoneMeshProps m_Props;
		std::filesystem::path m_Filepath;

		Asset<Mesh> m_PreviewMesh;

	public:
		BoneMeshResourceImporter();

		virtual void OnResourceOpen(const std::filesystem::path& filepath) override;
		virtual void OnImportOpen(ryml::NodeRef& node) override;
		virtual void OnRender() override;
		virtual void OnImport(ryml::NodeRef& node) override;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) override;
	};
}