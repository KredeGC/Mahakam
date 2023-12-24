#pragma once

#include "ResourceImporter.h"

#include "Mahakam/Renderer/MeshProps.h"

namespace Mahakam
{
	class Mesh;

	class CubeMeshResourceImporter : public ResourceImporter
	{
	private:
		CubeMeshProps m_Props;

		Asset<Mesh> m_PreviewMesh;

	public:
		CubeMeshResourceImporter();

		virtual void OnResourceOpen(const std::filesystem::path& filepath) override {} // Unused
		virtual void OnImportOpen(ryml::NodeRef& node) override;
		virtual void OnRender() override;
		virtual void OnImport(ryml::NodeRef& node) override;

		virtual Asset<void> CreateAsset(ryml::NodeRef& node) override;

	private:
		CubeMeshProps DeserializeProps(ryml::NodeRef& node);
	};
}