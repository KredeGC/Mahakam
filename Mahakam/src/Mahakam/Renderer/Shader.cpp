#include "Mahakam/mhpch.h"
#include "Shader.h"

#include "RendererAPI.h"
#include "ShaderUtility.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/Headless/HeadlessShader.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <ryml/rapidyaml-0.4.1.hpp>

#define ENABLE_HLSL
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <glslang/SPIRV/GlslangToSpv.h>

namespace Mahakam
{
	static const TBuiltInResource DefaultTBuiltInResource = {
		/* .MaxLights = */ 32,
		/* .MaxClipPlanes = */ 6,
		/* .MaxTextureUnits = */ 32,
		/* .MaxTextureCoords = */ 32,
		/* .MaxVertexAttribs = */ 64,
		/* .MaxVertexUniformComponents = */ 4096,
		/* .MaxVaryingFloats = */ 64,
		/* .MaxVertexTextureImageUnits = */ 32,
		/* .MaxCombinedTextureImageUnits = */ 80,
		/* .MaxTextureImageUnits = */ 32,
		/* .MaxFragmentUniformComponents = */ 4096,
		/* .MaxDrawBuffers = */ 32,
		/* .MaxVertexUniformVectors = */ 128,
		/* .MaxVaryingVectors = */ 8,
		/* .MaxFragmentUniformVectors = */ 16,
		/* .MaxVertexOutputVectors = */ 16,
		/* .MaxFragmentInputVectors = */ 15,
		/* .MinProgramTexelOffset = */ -8,
		/* .MaxProgramTexelOffset = */ 7,
		/* .MaxClipDistances = */ 8,
		/* .MaxComputeWorkGroupCountX = */ 65535,
		/* .MaxComputeWorkGroupCountY = */ 65535,
		/* .MaxComputeWorkGroupCountZ = */ 65535,
		/* .MaxComputeWorkGroupSizeX = */ 1024,
		/* .MaxComputeWorkGroupSizeY = */ 1024,
		/* .MaxComputeWorkGroupSizeZ = */ 64,
		/* .MaxComputeUniformComponents = */ 1024,
		/* .MaxComputeTextureImageUnits = */ 16,
		/* .MaxComputeImageUniforms = */ 8,
		/* .MaxComputeAtomicCounters = */ 8,
		/* .MaxComputeAtomicCounterBuffers = */ 1,
		/* .MaxVaryingComponents = */ 60,
		/* .MaxVertexOutputComponents = */ 64,
		/* .MaxGeometryInputComponents = */ 64,
		/* .MaxGeometryOutputComponents = */ 128,
		/* .MaxFragmentInputComponents = */ 128,
		/* .MaxImageUnits = */ 8,
		/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
		/* .MaxCombinedShaderOutputResources = */ 8,
		/* .MaxImageSamples = */ 0,
		/* .MaxVertexImageUniforms = */ 0,
		/* .MaxTessControlImageUniforms = */ 0,
		/* .MaxTessEvaluationImageUniforms = */ 0,
		/* .MaxGeometryImageUniforms = */ 0,
		/* .MaxFragmentImageUniforms = */ 8,
		/* .MaxCombinedImageUniforms = */ 8,
		/* .MaxGeometryTextureImageUnits = */ 16,
		/* .MaxGeometryOutputVertices = */ 256,
		/* .MaxGeometryTotalOutputComponents = */ 1024,
		/* .MaxGeometryUniformComponents = */ 1024,
		/* .MaxGeometryVaryingComponents = */ 64,
		/* .MaxTessControlInputComponents = */ 128,
		/* .MaxTessControlOutputComponents = */ 128,
		/* .MaxTessControlTextureImageUnits = */ 16,
		/* .MaxTessControlUniformComponents = */ 1024,
		/* .MaxTessControlTotalOutputComponents = */ 4096,
		/* .MaxTessEvaluationInputComponents = */ 128,
		/* .MaxTessEvaluationOutputComponents = */ 128,
		/* .MaxTessEvaluationTextureImageUnits = */ 16,
		/* .MaxTessEvaluationUniformComponents = */ 1024,
		/* .MaxTessPatchComponents = */ 120,
		/* .MaxPatchVertices = */ 32,
		/* .MaxTessGenLevel = */ 64,
		/* .MaxViewports = */ 16,
		/* .MaxVertexAtomicCounters = */ 0,
		/* .MaxTessControlAtomicCounters = */ 0,
		/* .MaxTessEvaluationAtomicCounters = */ 0,
		/* .MaxGeometryAtomicCounters = */ 0,
		/* .MaxFragmentAtomicCounters = */ 8,
		/* .MaxCombinedAtomicCounters = */ 8,
		/* .MaxAtomicCounterBindings = */ 1,
		/* .MaxVertexAtomicCounterBuffers = */ 0,
		/* .MaxTessControlAtomicCounterBuffers = */ 0,
		/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
		/* .MaxGeometryAtomicCounterBuffers = */ 0,
		/* .MaxFragmentAtomicCounterBuffers = */ 1,
		/* .MaxCombinedAtomicCounterBuffers = */ 1,
		/* .MaxAtomicCounterBufferSize = */ 16384,
		/* .MaxTransformFeedbackBuffers = */ 4,
		/* .MaxTransformFeedbackInterleavedComponents = */ 64,
		/* .MaxCullDistances = */ 8,
		/* .MaxCombinedClipAndCullDistances = */ 8,
		/* .MaxSamples = */ 4,
		/* .maxMeshOutputVerticesNV = */ 256,
		/* .maxMeshOutputPrimitivesNV = */ 512,
		/* .maxMeshWorkGroupSizeX_NV = */ 32,
		/* .maxMeshWorkGroupSizeY_NV = */ 1,
		/* .maxMeshWorkGroupSizeZ_NV = */ 1,
		/* .maxTaskWorkGroupSizeX_NV = */ 32,
		/* .maxTaskWorkGroupSizeY_NV = */ 1,
		/* .maxTaskWorkGroupSizeZ_NV = */ 1,
		/* .maxMeshViewCountNV = */ 4,
		/* .maxMeshOutputVerticesEXT = */ 256,
		/* .maxMeshOutputPrimitivesEXT = */ 256,
		/* .maxMeshWorkGroupSizeX_EXT = */ 128,
		/* .maxMeshWorkGroupSizeY_EXT = */ 128,
		/* .maxMeshWorkGroupSizeZ_EXT = */ 128,
		/* .maxTaskWorkGroupSizeX_EXT = */ 128,
		/* .maxTaskWorkGroupSizeY_EXT = */ 128,
		/* .maxTaskWorkGroupSizeZ_EXT = */ 128,
		/* .maxMeshViewCountEXT = */ 4,
		/* .maxDualSourceDrawBuffersEXT = */ 1,

		/* .limits = */ {
			/* .nonInductiveForLoops = */ 1,
			/* .whileLoops = */ 1,
			/* .doWhileLoops = */ 1,
			/* .generalUniformIndexing = */ 1,
			/* .generalAttributeMatrixVectorIndexing = */ 1,
			/* .generalVaryingIndexing = */ 1,
			/* .generalSamplerIndexing = */ 1,
			/* .generalVariableIndexing = */ 1,
			/* .generalConstantMatrixVectorIndexing = */ 1,
	} };

	static ShaderStage ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return ShaderStage::Vertex;
		else if (type == "fragment" || type == "pixel")
			return ShaderStage::Fragment;

		MH_WARN("Shader stage {0} not supported!", type);
		return ShaderStage::None;
	}

	UnorderedMap<ShaderStage, std::string> Shader::ParseGLSLFile(const std::string& source)
	{
		MH_PROFILE_FUNCTION();

		UnorderedMap<ShaderStage, std::string> sources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			MH_ASSERT(eol != std::string::npos, "Syntax error!");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			MH_ASSERT(ShaderTypeFromString(type) != ShaderStage::None, "Invalid shader type!");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			sources.insert({ ShaderTypeFromString(type), source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos)) });
		}

		return sources;
	}

	std::string Shader::ParseDefaultValue(const ryml::NodeRef& node)
	{
		if (node.is_seq())
		{
			std::stringstream value;
			for (auto iter = node.begin(); iter != node.end();)
			{
				value << ParseDefaultValue(*iter);
				if (++iter != node.end())
					value << ",";
			}
			return "[" + value.str() + "]";
		}
		// TODO: Maps
		else
		{
			ryml::csubstr val = node.val();
			return std::string(val.str, val.size());
		}
	}

	bool Shader::ParseYAMLFile(const std::filesystem::path& filepath, UnorderedMap<std::string, SourceDefinition>& sources, UnorderedMap<std::string, ShaderProperty>& properties)
	{
		TrivialVector<char> buffer;

		if (!FileUtility::ReadFile(filepath, buffer))
			return false;

		try
		{
			ryml::Tree tree = ryml::parse_in_arena(ryml::csubstr(buffer.data(), buffer.size()));

			ryml::NodeRef root = tree.rootref();

			if (!root.valid())
			{
				MH_WARN("Loaded empty shader file {0}!", filepath);
				return false;
			}

			if (root.has_child("Properties"))
			{
				ryml::NodeRef propertiesNode = root["Properties"];

				MH_INFO("Loading properties for shader: {0}", filepath.string());
				for (auto propertyNode : propertiesNode)
				{
					ryml::csubstr key = propertyNode.key();
					std::string propertyName(key.str, key.size());

					ShaderPropertyType propertyType = ShaderPropertyType::Default;
					if (propertyNode.has_child("Type"))
					{
						ryml::NodeRef typeNode = propertyNode["Type"];
						std::string typeString;
						typeNode >> typeString;

						if (typeString == "Color")			propertyType = ShaderPropertyType::Color;
						else if (typeString == "HDR")		propertyType = ShaderPropertyType::HDR;
						else if (typeString == "Vector")	propertyType = ShaderPropertyType::Vector;
						else if (typeString == "Range")		propertyType = ShaderPropertyType::Range;
						else if (typeString == "Drag")		propertyType = ShaderPropertyType::Drag;
						else if (typeString == "Texture")	propertyType = ShaderPropertyType::Texture;
						else if (typeString == "Normal")	propertyType = ShaderPropertyType::Normal;
						else if (typeString == "Default")	propertyType = ShaderPropertyType::Default;
					}

					float min = 0;
					if (propertyNode.has_child("Min"))
						propertyNode["Min"] >> min;

					float max = 0;
					if (propertyNode.has_child("Max"))
						propertyNode["Max"] >> max;

					std::string defaultValue;
					if (propertyNode.has_child("Default"))
						defaultValue = ParseDefaultValue(propertyNode["Default"]);

					properties[propertyName] = { propertyType, ShaderDataType::None, min, max, "Value: " + defaultValue, 1, 0 };

					MH_INFO("  {0}: {1}", propertyName, defaultValue);
				}
			}

			// Read shader passes
			if (root.has_child("Passes"))
			{
				ryml::NodeRef passesNode = root["Passes"];

				for (auto shaderPassNode : passesNode)
				{
					ryml::csubstr key = shaderPassNode.key();
					std::string shaderPassName(key.str, key.size());

					// Read shaderpass defines
					std::stringstream shaderPassDefines;
					if (shaderPassNode.has_child("Defines"))
					{
						auto definesNode = shaderPassNode["Defines"];
						for (auto defineNode : definesNode)
						{
							std::string define;
							defineNode >> define;
							shaderPassDefines << "#define " + define + "\n";
						}
					}

					// Read and compile include files
					if (shaderPassNode.has_child("Includes"))
					{
						ryml::NodeRef includesNode = shaderPassNode["Includes"];

						// Read and parse source files
						std::stringstream source;
						for (auto includeNode : includesNode)
						{
							std::string shaderPath;
							includeNode >> shaderPath;

							source << ShaderUtility::ReadFile(shaderPath);
						}

						auto glslSources = ParseGLSLFile(source.str());

						// Output shader source for each pass and stage
						sources.insert({ shaderPassName, SourceDefinition{ glslSources, shaderPassDefines.str() } });
					}
				}
			}

			return true;
		}
		catch (std::runtime_error const& e)
		{
			MH_WARN("Shader encountered exception trying to parse YAML file {0}: {1}", filepath, e.what());
		}

		return false;
	}

	bool Shader::CompileSPIRV(UnorderedMap<ShaderStage, std::vector<uint32_t>>& spirv, const SourceDefinition& sourceDef)
	{
		glslang::InitializeProcess();

		glslang::TProgram* program = Allocator::New<glslang::TProgram>();

		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgEnhanced);

		bool success = true;

		// Parse each shader stage
		TrivialVector<glslang::TShader*> shaders;
		shaders.reserve(sourceDef.Sources.size());
		for (auto& source : sourceDef.Sources)
		{
			glslang::TShader* shader = Allocator::New<glslang::TShader>((EShLanguage)ShaderStageToEShLanguage(source.first));
			const char* shaderStrings[1];

			std::string sortedSource = ShaderUtility::SortIncludes(source.second);

			shaderStrings[0] = sortedSource.c_str();

			// Set preprocessor defines
			shader->setPreamble(sourceDef.Defines.c_str());

			// Set target and version
			shader->setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EShTargetClientVersion::EShTargetOpenGL_450);

			// Set source code
			shader->setStrings(shaderStrings, 1);

			// TODO: Use HLSL instead of GLSL
			/*shader.setEnvInput(glslang::EShSourceHlsl,
				stage, glslang::EShClient::EShClientOpenGL, 100);
			shader.setEnvTargetHlslFunctionality1();*/

			if (!shader->parse(&DefaultTBuiltInResource, 100, false, messages))
			{
				MH_WARN(shader->getInfoLog());
				MH_WARN(shader->getInfoDebugLog());

				success = false;
			}

			shaders.push_back(shader);

			program->addShader(shader);
		}

		// Link program with shaders
		if (!program->link(messages))
		{
			MH_WARN(program->getInfoLog());
			MH_WARN(program->getInfoDebugLog());

			success = false;
		}

		// Retrieve SPIR-V
		if (success)
		{
			for (int stage = 0; stage < EShLangCount; stage++)
			{
				// Output into 1 big SPIR-V binary
				if (glslang::TIntermediate* ptr = program->getIntermediate((EShLanguage)stage))
				{
					ShaderStage stageEnum = EShLanguageToShaderStage(stage);
					spirv.insert({ stageEnum, std::vector<uint32_t>() });
					glslang::GlslangToSpv(*ptr, spirv[stageEnum]);
				}
			}
		}

		// Cleanup
		Allocator::Delete(program);
		for (auto& shader : shaders)
			Allocator::Delete(shader);

		glslang::FinalizeProcess();


		// TEMP
		//if (success)
		//{
		//	// GLSL
		//	{
		//		spirv_cross::CompilerGLSL glsl(spirv[ShaderStage::Vertex]);

		//		spirv_cross::CompilerGLSL::Options options;
		//		options.version = 430;
		//		glsl.set_common_options(options);

		//		// Compile to GLSL, ready to give to GL driver.
		//		std::string source = glsl.compile();

		//		MH_TRACE(source);
		//	}

		//	// HLSL
		//	{
		//		spirv_cross::CompilerHLSL hlsl(spirv[ShaderStage::Vertex]);

		//		spirv_cross::CompilerHLSL::Options options;
		//		options.shader_model = 40;
		//		hlsl.set_hlsl_options(options);

		//		// Compile to HLSL, ready to give to DX driver.
		//		std::string source = hlsl.compile();

		//		MH_TRACE(source);
		//	}
		//}

		return success;
	}

	ShaderDataType SPIRVDimToShaderDataType(spv::Dim dimension)
	{
		switch (dimension)
		{
		case spv::Dim::Dim2D:
			return ShaderDataType::Sampler2D;
		case spv::Dim::DimCube:
			return ShaderDataType::SamplerCube;
		default:
			MH_BREAK("Unknown SPIR V dimension provided!");
			return ShaderDataType::None;
		}
	}

	ShaderDataType SPIRTypeToShaderDataType(spirv_cross::SPIRType::BaseType type, uint32_t size)
	{
		switch (type)
		{
		case spirv_cross::SPIRType::BaseType::Float:
			switch (size)
			{
			case 1: return ShaderDataType::Float;
			case 2: return ShaderDataType::Float2;
			case 3: return ShaderDataType::Float3;
			case 4: return ShaderDataType::Float4;
			default:
				MH_BREAK("Unknown SPIR V data type provided!");
				return ShaderDataType::None;
			}
		default:
			MH_BREAK("Unknown SPIR V data type provided!");
			return ShaderDataType::None;
		}
	}

	uint32_t Shader::ReflectSPIRV(const std::vector<uint32_t>& spirv, UnorderedMap<std::string, ShaderProperty>& properties)
	{
		uint32_t uniform_size = 0;

		spirv_cross::Compiler comp(spirv);

		spirv_cross::ShaderResources resources = comp.get_shader_resources();

		// Reflect samplers
		for (auto& resource : resources.sampled_images)
		{
			const spirv_cross::SPIRType& type = comp.get_type(resource.base_type_id);

			const std::string& propertyName = resource.name;

			// Set property
			auto& property = properties[propertyName];
			property.DataType = SPIRVDimToShaderDataType(type.image.dim);
			property.Count = static_cast<uint32_t>(type.array.size());
			property.Offset = 0;
		}

		std::string uboName = "Uniforms";

		// Reflect uniform values
		for (auto& resource : resources.uniform_buffers)
		{
			if (resource.name != uboName)
				continue;

			const spirv_cross::SPIRType& type = comp.get_type(resource.base_type_id);

			uniform_size = static_cast<uint32_t>(comp.get_declared_struct_size(type));

			for (size_t i = 0; i < type.member_types.size(); i++)
			{
				const spirv_cross::SPIRType& member_type = comp.get_type(type.member_types[i]);
				const std::string& member_name = comp.get_member_name(resource.base_type_id, i);
				size_t member_offset = comp.type_struct_member_offset(type, i);

				ShaderDataType dataType = SPIRTypeToShaderDataType(member_type.basetype, member_type.vecsize);

				std::string propertyName = uboName + "." + member_name;

				// Set property
				auto& property = properties[propertyName];
				property.DataType = dataType;
				property.Count = static_cast<uint32_t>(member_type.array.size());
				property.Offset = member_offset;
			}

			break;
		}

		return uniform_size;
	}

	//Asset<Shader> Shader::Create(const std::string& filepath, const std::initializer_list<std::string>& keywords)
	MH_DEFINE_FUNC(Shader::CreateFilepath, Asset<Shader>, const std::filesystem::path& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateAsset<HeadlessShader>(filepath);
		case RendererAPI::API::OpenGL:
			return CreateAsset<OpenGLShader>(filepath);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};

	ShaderStage EShLanguageToShaderStage(uint32_t stage)
	{
		switch (stage)
		{
		case 0:
			return ShaderStage::Vertex;
		case 4:
			return ShaderStage::Fragment;
		default:
			MH_BREAK("Unknown shader data type!");
			return ShaderStage::None;
		}
	}

	uint32_t ShaderStageToEShLanguage(ShaderStage stage)
	{
		switch (stage)
		{
		case ShaderStage::Vertex:
			return 0;
		case ShaderStage::Fragment:
			return 4;
		default:
			MH_BREAK("Unknown shader data type!");
			return 0;
		}
	}

	uint32_t ShaderDataTypeComponentCount(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 1;
		case ShaderDataType::Float2:
			return 2;
		case ShaderDataType::Float3:
			return 3;
		case ShaderDataType::Float4:
			return 4;
		case ShaderDataType::Mat3:
			return 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4;
		case ShaderDataType::Int:
			return 1;
		case ShaderDataType::Int2:
			return 2;
		case ShaderDataType::Int3:
			return 3;
		case ShaderDataType::Int4:
			return 4;
		case ShaderDataType::Bool:
			return 1;
		default:
			MH_BREAK("Unknown shader data type!");
			return 0;
		}
	}

	uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 8;
		case ShaderDataType::Float3:
			return 12;
		case ShaderDataType::Float4:
			return 16;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
		case ShaderDataType::Int:
			return 4;
		case ShaderDataType::Int2:
			return 8;
		case ShaderDataType::Int3:
			return 12;
		case ShaderDataType::Int4:
			return 16;
		case ShaderDataType::Bool:
			return 1;
		default:
			MH_BREAK("Unknown shader data type!");
			return 0;
		}
	}
}