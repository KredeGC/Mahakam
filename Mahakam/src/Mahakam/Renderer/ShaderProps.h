#pragma once

#include "ShaderDataTypes.h"

#include <string>

namespace Mahakam
{
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
	};
}