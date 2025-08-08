#pragma once

#include "AssetSerializeTraits.h"

#include "Mahakam/Renderer/Shader.h"

#include "Mahakam/BinarySerialization/ContainerSerialization.h"
#include "Mahakam/BinarySerialization/EnumSerialization.h"
#include "Mahakam/BinarySerialization/MathSerialization.h"
#include "Mahakam/BinarySerialization/StringSerialization.h"

namespace Mahakam::Serialization
{
	template<>
	struct SerializeTraits<ShaderProperty>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, ShaderProperty> shaderProperty) noexcept
		{
			return stream.serialize(shaderProperty.PropertyType)
				&& stream.serialize(shaderProperty.DataType)
				&& stream.serialize(shaderProperty.Min)
				&& stream.serialize(shaderProperty.Max)
				&& stream.serialize(shaderProperty.DefaultString)
				&& stream.serialize(shaderProperty.Count)
				&& stream.serialize(shaderProperty.Offset);
		}
	};

	template<>
	struct SerializeTraits<ShaderData>
	{
		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, ShaderData> shaderData) noexcept
		{
			return stream.serialize(shaderData.GetShaderData())
				&& stream.serialize(shaderData.GetOffsets());
		}
	};

	template<>
	struct AssetSerializeTraits<Shader>
	{
		template<typename Stream>
		static std::vector<Asset<void>> dependencies(Stream& reader) noexcept
		{
			return {};
		}

		template<typename Stream>
		typename utility::is_writing_t<Stream>
			static serialize(Stream& writer, inout<Stream, Asset<Shader>> shader) noexcept
		{
			return writer.serialize(shader->GetProperties())
				&& writer.serialize(shader->GetShaderData());
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
			static serialize(Stream& reader, inout<Stream, Asset<Shader>> shader) noexcept
		{
			UnorderedMap<std::string, ShaderProperty> shaderProperties;
			UnorderedMap<std::string, ShaderData> shaderData;

			if (!reader.serialize(shaderProperties) || !reader.serialize(shaderData))
				return false;

			shader = Shader::Create(std::move(shaderProperties), std::move(shaderData));

			return true;
		}
	};
}