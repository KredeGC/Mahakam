#include "Mahakam/mhpch.h"
#include "Application.h"

#include "Input.h"
#include "Log.h"
#include "Profiler.h"
#include "SharedLibrary.h"

#include "Mahakam/Audio/AudioEngine.h"

#include "Mahakam/Physics/PhysicsEngine.h"

#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/RendererAPI.h"


// TEMP
#define ENABLE_HLSL
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>
#include <glslang/SPIRV/GlslangToSpv.h>

namespace Mahakam
{
	const TBuiltInResource DefaultTBuiltInResource = {
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


	Application* Application::instance = nullptr;

	Application::Application(const WindowProps& props)
	{
		MH_PROFILE_FUNCTION();

		MH_CORE_ASSERT(!instance, "Application instance already created!");
		instance = this;

		window = Window::Create(props);
		window->SetEventCallback(MH_BIND_EVENT(Application::OnEvent));

		Renderer::Init(props.Width, props.Height);

		AudioEngine::Init();

		PhysicsEngine::Init();

		if (RendererAPI::GetAPI() != RendererAPI::API::None)
		{
			imGuiLayer = new ImGuiLayer();
			PushOverlay(imGuiLayer);
		}

		// TEMP
		glslang::InitializeProcess();
		
		EShLanguage stage = EShLangVertex;
		glslang::TShader shader(stage);
		const char* shaderStrings[1];
		
		EShMessages messages = (EShMessages)(EShMsgDefault | EShMsgSpvRules | EShMsgEnhanced);

		shaderStrings[0] = R"V0G0N(#version 430 core

			layout(location = 0) in vec3 i_Pos;
			layout(location = 1) in vec2 i_UV;

			layout(location = 0) out vec2 v_UV;

			layout(std140, binding = 0) uniform Uniforms {
				vec4 u_UVTransform;
				vec3 u_Color;
				vec3 u_EmissionColor;
				float u_MetallicMul;
				float u_RoughnessMul;
			};

			void main() {
				gl_Position = vec4(i_Pos, 1.0);
				v_UV = i_UV;
			})V0G0N";

		shader.setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EShTargetClientVersion::EShTargetOpenGL_450);
		shader.setStrings(shaderStrings, 1);

		/*shader.setEnvInput(glslang::EShSourceHlsl,
			stage, glslang::EShClient::EShClientOpenGL, 100);
		shader.setEnvTargetHlslFunctionality1();*/

		if (!shader.parse(&DefaultTBuiltInResource, 100, false, messages))
		{
			MH_CORE_ERROR(shader.getInfoLog());
			MH_CORE_BREAK(shader.getInfoDebugLog());
		}

		glslang::TProgram program;
		program.addShader(&shader);

		if (!program.link(messages))
		{
			MH_CORE_ERROR(program.getInfoLog());
			MH_CORE_BREAK(program.getInfoDebugLog());
		}

		std::vector<uint32_t> spirv;

		glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

		glslang::FinalizeProcess();

		// GLSL
		{
			spirv_cross::CompilerGLSL glsl(spirv);

			spirv_cross::CompilerGLSL::Options options;
			options.version = 430;
			options.vulkan_semantics = true;
			glsl.set_common_options(options);

			// Compile to GLSL, ready to give to GL driver.
			std::string source = glsl.compile();

			MH_CORE_TRACE(source);
		}

		// HLSL
		{
			spirv_cross::CompilerHLSL hlsl(spirv);

			spirv_cross::CompilerHLSL::Options options;
			options.shader_model = 40;
			hlsl.set_hlsl_options(options);

			// Compile to HLSL, ready to give to DX driver.
			std::string source = hlsl.compile();

			MH_CORE_TRACE(source);
		}

		MH_BREAKPOINT();
	}

	Application::~Application()
	{
		MH_PROFILE_FUNCTION();

		for (Layer* layer : layerStack)
			layer->OnDetach();

		AudioEngine::Shutdown();

		PhysicsEngine::Shutdown();

		Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (running)
		{
			double time = window->GetTime();
			Timestep timestep = (float)(time - lastFrameTime);
			lastFrameTime = time;

			if (!minimized)
			{
				for (Layer* layer : layerStack)
					layer->OnUpdate(timestep);
			}

			if (imGuiLayer)
			{
				imGuiLayer->Begin();
				for (Layer* layer : layerStack)
					layer->OnImGuiRender();
				imGuiLayer->End();
			}

			window->OnUpdate();

#ifdef MH_ENABLE_PROFILING
			Profiler::ClearResults();
#endif
		}
	}

	void Application::Close()
	{
		running = false;
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.DispatchEvent<WindowCloseEvent>(MH_BIND_EVENT(Application::OnWindowClose));
		dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(Application::OnWindowResize));

		for (auto iter = layerStack.end(); iter != layerStack.begin();)
		{
			if (event.handled)
				break;
			(*--iter)->OnEvent(event);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		layer->OnDetach();
		layerStack.PopLayer(layer);

		delete layer;
	}

	void Application::PopOverlay(Layer* overlay)
	{
		overlay->OnDetach();
		layerStack.PopOverlay(overlay);

		delete overlay;
	}

	Application* Application::GetInstance()
	{
		return instance;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		if (event.GetWidth() == 0 || event.GetHeight() == 0)
		{
			minimized = true;
			return false;
		}

		minimized = false;

		Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

		return false;
	}
}