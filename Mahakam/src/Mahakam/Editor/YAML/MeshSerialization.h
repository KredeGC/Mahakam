#pragma once

#include "Mahakam/Renderer/MeshProps.h"

#include <ryml/rapidyaml-0.4.1.hpp>

namespace c4::yml
{
	// MeshProps
	void write(ryml::NodeRef* n, Mahakam::MeshProps const& val);
	bool read(ryml::NodeRef const& n, Mahakam::MeshProps* val);

	// TODO: Might not be needed
	// MeshNode
	void write(ryml::NodeRef* n, Mahakam::MeshNode const& val);
	bool read(ryml::NodeRef const& n, Mahakam::MeshNode* val);
}