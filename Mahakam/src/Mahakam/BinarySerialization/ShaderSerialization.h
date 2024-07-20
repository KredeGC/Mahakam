#pragma once

#include "Mahakam/Renderer/Shader.h"

#include "ContainerSerialization.h"
#include "MathSerialization.h"

#include <bitstream.h>

namespace bitstream
{
	template<>
	struct serialize_traits<Mahakam::ShaderProperty>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Mahakam::ShaderProperty> shaderProperty) noexcept
		{
			BS_ASSERT(stream.serialize(shaderProperty.PropertyType));
			BS_ASSERT(stream.serialize(shaderProperty.DataType));
			BS_ASSERT(stream.serialize(shaderProperty.Min));
			BS_ASSERT(stream.serialize(shaderProperty.Max));
			BS_ASSERT(stream.serialize(shaderProperty.DefaultString, 256));
			BS_ASSERT(stream.serialize(shaderProperty.Count));
			BS_ASSERT(stream.serialize(shaderProperty.Offset));

			return true;
		}
	};

	template<>
	struct serialize_traits<Mahakam::ShaderData>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, bitstream::inout<Stream, Mahakam::ShaderData> shaderData) noexcept
		{
			BS_ASSERT(stream.serialize(shaderData.GetShaderData()));
			BS_ASSERT(stream.serialize(shaderData.GetOffsets()));

			return true;
		}
	};

	template<>
	struct serialize_traits<Mahakam::Shader>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Mahakam::Asset<Mahakam::Shader>> shader) noexcept
		{
			using namespace Mahakam;

			if constexpr (Stream::writing)
			{
				BS_ASSERT(stream.serialize(shader->GetProperties()));
				BS_ASSERT(stream.serialize(shader->GetShaderData()));
			}
			else
			{
				UnorderedMap<std::string, ShaderProperty> shaderProperties;
				BS_ASSERT(stream.serialize(shaderProperties));

				UnorderedMap<std::string, ShaderData> shaderData;
				BS_ASSERT(stream.serialize(shaderData));

				shader = Shader::Create(std::move(shaderProperties), std::move(shaderData));
			}

			return true;
		}
	};
}