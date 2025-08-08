#pragma once

#include "SerializeTraits.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Mahakam::Serialization
{
	template<>
	struct SerializeTraits<float>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, float> value)
		{
			return stream.serialize_elements(&value, 1);
		}
	};

	template<>
	struct SerializeTraits<glm::vec2>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, glm::vec2> value)
		{
			return stream.serialize_elements(&value, 1);
		}
	};

	template<>
	struct SerializeTraits<glm::vec3>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, glm::vec3> value)
		{
			return stream.serialize_elements(&value, 1);
		}
	};

	template<>
	struct SerializeTraits<glm::vec4>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, glm::vec4> value)
		{
			return stream.serialize_elements(&value, 1);
		}
	};

	template<>
	struct SerializeTraits<glm::quat>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, glm::quat> value)
		{
			return stream.serialize_elements(&value, 1);
		}
	};

	template<>
	struct SerializeTraits<glm::mat3>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, glm::mat3> value)
		{
			return stream.serialize_elements(&value, 1);
		}
	};

	template<>
	struct SerializeTraits<glm::mat4>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, glm::mat4> value)
		{
			return stream.serialize_elements(&value, 1);
		}
	};
}