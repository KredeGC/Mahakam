#include "mhpch.h"
#include "SharedLibrary.h"

#define MH_EXPORT_FUNC(signature, func) funcPointers[i++] = static_cast<signature>(func);
#define MH_IMPORT_FUNC(signature) sh_##signature = static_cast<signature>(ptrs[i++]);

namespace Mahakam
{
	SharedLibrary::SharedLibrary(const char* filepath) : filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

#if defined(MH_PLATFORM_WINDOWS)
		size_t size = strlen(filepath) + 1;
		wchar_t* wa = new wchar_t[size];
		mbstowcs(wa, filepath, size);

		handle = LoadLibrary(wa);

		delete[] wa;
#elif defined(MH_PLATFORM_LINUX)
		handle = dlopen(filepath, RTLD_NOW);
#endif

		MH_CORE_ASSERT(handle, "Failed to open shared library!");

		if (!initialized)
			ExportFuncPointers();

		auto loadPtr = GetFunction<void, void**>("Load");

		if (loadPtr)
			loadPtr(funcPointers);
	}

	SharedLibrary::~SharedLibrary()
	{
		MH_PROFILE_FUNCTION();

		auto unloadPtr = GetFunction<void>("Unload");

		if (unloadPtr)
			unloadPtr();

#if defined(MH_PLATFORM_WINDOWS)
		MH_CORE_ASSERT(FreeLibrary(handle), "Failed to close shared library!");
#elif defined(MH_PLATFORM_LINUX)
		MH_CORE_ASSERT(dlclose(handle), "Failed to close shared library!");
#endif
	}

	void SharedLibrary::ExportFuncPointers()
	{
		MH_PROFILE_FUNCTION();

		initialized = true;

		int i = 0;

		// Animation
		MH_EXPORT_FUNC(AnimationLoad, Animation::Load);

		// UniformBuffer
		MH_EXPORT_FUNC(UniformBufferCreate, UniformBuffer::Create);

		// StorageBuffer
		MH_EXPORT_FUNC(StorageBufferCreate, StorageBuffer::Create);

		// ComputeShader
		MH_EXPORT_FUNC(ComputeShaderCreate, ComputeShader::Create);

		// FrameBuffer
		MH_EXPORT_FUNC(FrameBufferCreate, FrameBuffer::Create);

		// GL
		MH_EXPORT_FUNC(GLGetInstance, GL::GetInstance);

		// Log
		MH_EXPORT_FUNC(EngineLogger, Log::GetEngineLogger);
		MH_EXPORT_FUNC(GameLogger, Log::GetGameLogger);

		// Material
		MH_EXPORT_FUNC(MaterialCopy, Material::Copy);
		MH_EXPORT_FUNC(MaterialCreate, Material::Create);

		// Mesh
		MH_EXPORT_FUNC(MeshCreate, Mesh::Create);
		MH_EXPORT_FUNC(MeshLoad, Mesh::LoadModel);

		// Profiler
		MH_EXPORT_FUNC(ProfilerCreate, Profiler::Create);
		MH_EXPORT_FUNC(ProfilerAddResult, Profiler::AddResult);
		MH_EXPORT_FUNC(ProfilerClear, Profiler::ClearResults);
		MH_EXPORT_FUNC(ProfilerGetResults, Profiler::GetResults);

		// RenderBuffer
		MH_EXPORT_FUNC(RenderBufferCreate, RenderBuffer::Create);

		// Renderer
		MH_EXPORT_FUNC(RendererGetInstance, Renderer::GetInstance);

		// Shader
		MH_EXPORT_FUNC(ShaderCreate, Shader::Create);

		// Texture2D
		MH_EXPORT_FUNC(Texture2DCreateProps, Texture2D::Create);
		MH_EXPORT_FUNC(Texture2DCreateFilepath, Texture2D::Create);

		// TextureCube
		MH_EXPORT_FUNC(TextureCubeCreateProps, TextureCube::Create);
		MH_EXPORT_FUNC(TextureCubeCreateFilepath, TextureCube::Create);
		MH_EXPORT_FUNC(TextureCubeCreatePrefilter, TextureCube::Create);

		MH_CORE_ASSERT(i == NUM_FUNC_PTRS, "SharedLibrary function count mismatch. Change NUM_FUNC_PTRS to reflect the new count");
	}

	void SharedLibrary::ImportFuncPointers(void* ptrs[NUM_FUNC_PTRS])
	{
		MH_PROFILE_FUNCTION();

		initialized = true;

		std::memcpy(funcPointers, ptrs, NUM_FUNC_PTRS * sizeof(void*));

		int i = 0;

		// Animation
		MH_IMPORT_FUNC(AnimationLoad);

		// UniformBuffer
		MH_IMPORT_FUNC(UniformBufferCreate);

		// StorageBuffer
		MH_IMPORT_FUNC(StorageBufferCreate);

		// ComputeShader
		MH_IMPORT_FUNC(ComputeShaderCreate);

		// FrameBuffer
		MH_IMPORT_FUNC(FrameBufferCreate);

		// GL
		MH_IMPORT_FUNC(GLGetInstance);

		// Log
		MH_IMPORT_FUNC(EngineLogger);
		MH_IMPORT_FUNC(GameLogger);

		// Material
		MH_IMPORT_FUNC(MaterialCopy);
		MH_IMPORT_FUNC(MaterialCreate);

		// Mesh
		MH_IMPORT_FUNC(MeshCreate);
		MH_IMPORT_FUNC(MeshLoad);

		// Profiler
		MH_IMPORT_FUNC(ProfilerCreate);
		MH_IMPORT_FUNC(ProfilerAddResult);
		MH_IMPORT_FUNC(ProfilerClear);
		MH_IMPORT_FUNC(ProfilerGetResults);

		// RenderBuffer
		MH_IMPORT_FUNC(RenderBufferCreate);

		// Renderer
		MH_IMPORT_FUNC(RendererGetInstance);

		// Shader
		MH_IMPORT_FUNC(ShaderCreate);

		// Texture2D
		MH_IMPORT_FUNC(Texture2DCreateProps);
		MH_IMPORT_FUNC(Texture2DCreateFilepath);

		// TextureCube
		MH_IMPORT_FUNC(TextureCubeCreateProps);
		MH_IMPORT_FUNC(TextureCubeCreateFilepath);
		MH_IMPORT_FUNC(TextureCubeCreatePrefilter);

		MH_CORE_ASSERT(i == NUM_FUNC_PTRS, "SharedLibrary function count mismatch. Change NUM_FUNC_PTRS to reflect the new count");
	}
}