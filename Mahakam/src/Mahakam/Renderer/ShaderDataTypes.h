#pragma once

#include <cstdint>

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

	ShaderStage EShLanguageToShaderStage(uint32_t stage);

	uint32_t ShaderStageToEShLanguage(ShaderStage stage);

	uint32_t ShaderDataTypeComponentCount(ShaderDataType type);

	uint32_t ShaderDataTypeSize(ShaderDataType type);
}