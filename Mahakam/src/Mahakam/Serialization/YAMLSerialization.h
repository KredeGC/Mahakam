#pragma once

#include "Mahakam/Asset/Asset.h"

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

#include <filesystem>

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

	// Filesyste,
	void write(ryml::NodeRef* n, std::filesystem::path const& val);

	bool read(ryml::NodeRef const& n, std::filesystem::path* val);

	// Asset
	template<typename T>
	void write(ryml::NodeRef* n, Mahakam::Asset<T> val)
	{
		*n << val.GetID();
	}

	template<typename T>
	bool read(ryml::NodeRef const& n, Mahakam::Asset<T>* val)
	{
		Mahakam::AssetDatabase::AssetID id;
		n >> id;
		*val = Mahakam::Asset<T>(id);
		return true;
	}
}

namespace Mahakam
{
	template<typename V>
	void SerializeYAMLNode(ryml::NodeRef& node, const c4::csubstr& name, const V& value)
	{
		node[name] << value;
	}

	template<typename V>
	bool DeserializeYAMLNode(ryml::NodeRef& node, const c4::csubstr& name, V& value)
	{
		if (!node.has_child(name))
			return false;

		node[name] >> value;

		return true;
	}
}