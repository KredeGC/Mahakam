#pragma once

#include "Mahakam/Editor/YAML/FilepathSerialization.h"

#include <ryml/rapidyaml-0.4.1.hpp>

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#include <magic_enum/magic_enum.hpp>

#include <filesystem>
#include <type_traits>

// TODO: Move into more specific headers
namespace c4::yml
{
	// GLM
	void write(ryml::NodeRef* n, glm::vec2 const& val);
	void write(ryml::NodeRef* n, glm::vec3 const& val);
	void write(ryml::NodeRef* n, glm::vec4 const& val);
	void write(ryml::NodeRef* n, glm::quat const& val);
	void write(ryml::NodeRef* n, glm::mat3 const& val);
	void write(ryml::NodeRef* n, glm::mat4 const& val);

	bool read(ryml::NodeRef const& n, glm::vec2* val);
	bool read(ryml::NodeRef const& n, glm::vec3* val);
	bool read(ryml::NodeRef const& n, glm::vec4* val);
	bool read(ryml::NodeRef const& n, glm::quat* val);
	bool read(ryml::NodeRef const& n, glm::mat3* val);
	bool read(ryml::NodeRef const& n, glm::mat4* val);
}

namespace Mahakam
{
	// Normal values
	template<typename V>
	typename std::enable_if_t<!std::is_enum_v<V>>
	SerializeYAMLNode(ryml::NodeRef& node, const c4::csubstr& name, const V& value)
	{
		node[name] << value;
	}

	template<typename V>
	typename std::enable_if_t<!std::is_enum_v<V>, bool>
	DeserializeYAMLNode(ryml::NodeRef& node, const c4::csubstr& name, V& value)
	{
		if (!node.has_child(name))
			return false;

		node[name] >> value;

		return true;
	}

	// Enums
	template<typename V>
	typename std::enable_if_t<std::is_enum_v<V>>
	SerializeYAMLNode(ryml::NodeRef& node, const c4::csubstr& name, V value)
	{
		std::string enum_name(magic_enum::enum_name(value));
		SerializeYAMLNode(node, name, enum_name);
	}

	template<typename V>
	typename std::enable_if_t<std::is_enum_v<V>, bool>
	DeserializeYAMLNode(ryml::NodeRef& node, const c4::csubstr& name, V& value)
	{
		std::string enum_name;
		if (!DeserializeYAMLNode(node, name, enum_name))
			return false;

		auto opt = magic_enum::enum_cast<V>(enum_name);
		if (!opt)
			return false;

		value = *opt;

		return true;
	}
}