#pragma once
#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Profiler.h"

#include "ShaderDataTypes.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace Mahakam::ShaderUtility
{
	// TODO: Make static?
	std::string SortIncludes(const std::string& source);
	std::string ReadFile(const std::filesystem::path& filepath);
	UnorderedMap<ShaderStage, std::string> ParseGLSLFile(const std::string& source);

	// TODO: Move into editor or similar?
	// It probably won't be needed during runtime
	bool ParseYAMLFile(const std::filesystem::path& filepath, UnorderedMap<std::string, ShaderSource>& sources, UnorderedMap<std::string, ShaderProperty>& properties);
	bool CompileSPIRV(UnorderedMap<ShaderStage, std::vector<uint32_t>>& spirv, const ShaderSource& source);
	ShaderData CompileSPIRV(const ShaderSource& source);
	uint32_t ReflectSPIRV(const std::vector<uint32_t>& spirv, UnorderedMap<std::string, ShaderProperty>& properties);

	// TODO: Make static?
	ShaderStage EShLanguageToShaderStage(uint32_t stage);
	uint32_t ShaderStageToEShLanguage(ShaderStage stage);

	ShaderDataType ShaderDataTypeBaseType(ShaderDataType type);
	uint32_t ShaderDataTypeComponentCount(ShaderDataType type);
	uint32_t ShaderDataTypeSize(ShaderDataType type);
}