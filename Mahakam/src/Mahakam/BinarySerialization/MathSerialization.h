#pragma once

#include <bitstream.h>

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtx/quaternion.hpp>

namespace bitstream
{
	template<>
	struct serialize_traits<glm::vec2>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, glm::vec2> value) noexcept
		{
			BS_ASSERT(stream.template serialize<float>(value[0]));
			BS_ASSERT(stream.template serialize<float>(value[1]));

			return true;
		}
	};

	template<>
	struct serialize_traits<glm::vec3>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, glm::vec3> value) noexcept
		{
			BS_ASSERT(stream.template serialize<float>(value[0]));
			BS_ASSERT(stream.template serialize<float>(value[1]));
			BS_ASSERT(stream.template serialize<float>(value[2]));

			return true;
		}
	};

	template<>
	struct serialize_traits<glm::vec4>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, glm::vec4> value) noexcept
		{
			BS_ASSERT(stream.template serialize<float>(value[0]));
			BS_ASSERT(stream.template serialize<float>(value[1]));
			BS_ASSERT(stream.template serialize<float>(value[2]));
			BS_ASSERT(stream.template serialize<float>(value[3]));

			return true;
		}
	};

	template<>
	struct serialize_traits<glm::mat3>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, glm::mat3> value) noexcept
		{
			BS_ASSERT(stream.template serialize<glm::vec3>(value[0]));
			BS_ASSERT(stream.template serialize<glm::vec3>(value[1]));
			BS_ASSERT(stream.template serialize<glm::vec3>(value[2]));

			return true;
		}
	};

	template<>
	struct serialize_traits<glm::mat4>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, glm::mat4> value) noexcept
		{
			BS_ASSERT(stream.template serialize<glm::vec4>(value[0]));
			BS_ASSERT(stream.template serialize<glm::vec4>(value[1]));
			BS_ASSERT(stream.template serialize<glm::vec4>(value[2]));
			BS_ASSERT(stream.template serialize<glm::vec4>(value[3]));

			return true;
		}
	};
}