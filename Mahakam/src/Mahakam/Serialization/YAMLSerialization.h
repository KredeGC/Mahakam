#pragma once

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

namespace glm
{
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