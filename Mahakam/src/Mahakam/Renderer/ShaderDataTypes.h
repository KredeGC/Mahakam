#pragma once

#include <cstdint>
#include <string>

namespace Mahakam
{
	enum class ShaderStage
	{
		None = 0,
		Vertex,
		Fragment
	};

	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Sampler2D,
		SamplerCube
	};

	enum class ShaderPropertyType
	{
		Color,
		HDR,
		Vector,
		Range,
		Drag,
		Texture,
		Normal,
		Default
	};

	struct ShaderProperty
	{
		ShaderPropertyType PropertyType;
		ShaderDataType DataType;
		float Min = 0;
		float Max = 0;
		std::string DefaultString;
		uint32_t Count = 1;
		int Offset = 0;
	};

	struct ShaderSource
	{
		UnorderedMap<ShaderStage, std::string> Sources;
		std::string Defines;
	};

	// TODO: Deprecated
	using SourceDefinition = ShaderSource;
}