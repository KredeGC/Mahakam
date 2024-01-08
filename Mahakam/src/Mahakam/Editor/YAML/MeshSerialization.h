#pragma once

#include "Mahakam/Renderer/MeshProps.h"

#include <ryml/rapidyaml-0.4.1.hpp>

namespace c4::yml
{
	// MeshProps
	void write(ryml::NodeRef* n, Mahakam::MeshProps const& val);
	bool read(ryml::NodeRef const& n, Mahakam::MeshProps* val);

	// BoneMeshProps
	void write(ryml::NodeRef* n, Mahakam::BoneMeshProps const& val);
	bool read(ryml::NodeRef const& n, Mahakam::BoneMeshProps* val);

	// CubeMeshProps
	void write(ryml::NodeRef* n, Mahakam::CubeMeshProps const& val);
	bool read(ryml::NodeRef const& n, Mahakam::CubeMeshProps* val);

	// CubeSphereMeshProps
	void write(ryml::NodeRef* n, Mahakam::CubeSphereMeshProps const& val);
	bool read(ryml::NodeRef const& n, Mahakam::CubeSphereMeshProps* val);

	// PlaneMeshProps
	void write(ryml::NodeRef* n, Mahakam::PlaneMeshProps const& val);
	bool read(ryml::NodeRef const& n, Mahakam::PlaneMeshProps* val);

	// UVSphereMeshProps
	void write(ryml::NodeRef* n, Mahakam::UVSphereMeshProps const& val);
	bool read(ryml::NodeRef const& n, Mahakam::UVSphereMeshProps* val);

	// TODO: Might not be needed
	// MeshNode
	void write(ryml::NodeRef* n, Mahakam::MeshNode const& val);
	bool read(ryml::NodeRef const& n, Mahakam::MeshNode* val);
}