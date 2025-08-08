#pragma once

#include <ryml/rapidyaml-0.4.1.hpp>

#include <filesystem>

namespace c4::yml
{
	void write(ryml::NodeRef* n, std::filesystem::path const& val);
	bool read(ryml::NodeRef const& n, std::filesystem::path* val);
}