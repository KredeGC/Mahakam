#include "mhpch.h"
#include "SharedLibrary.h"

#define MH_EXPORT_FUNC(signature, func) funcPointers[i++] = static_cast<signature>(func);
#define MH_IMPORT_FUNC(signature, variable) variable = static_cast<signature>(ptrs[i++]);

namespace Mahakam
{
	SharedLibrary::SharedLibrary(const char* filepath) : filepath(filepath)
	{
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
		initialized = true;

		int i = 0;

		// UniformBuffer
		MH_EXPORT_FUNC(UniformBufferCreate, UniformBuffer::Create);

		// StorageBuffer
		MH_EXPORT_FUNC(StorageBufferCreate, StorageBuffer::Create);

		// ComputeShader
		MH_EXPORT_FUNC(ComputeShaderCreate, ComputeShader::Create);

		// FrameBuffer
		MH_EXPORT_FUNC(FrameBufferCreate, FrameBuffer::Create);

		// RenderBuffer
		MH_EXPORT_FUNC(RenderBufferCreate, RenderBuffer::Create);

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
		initialized = true;

		std::memcpy(funcPointers, ptrs, NUM_FUNC_PTRS * sizeof(void*));

		int i = 0;

		// UniformBuffer
		MH_IMPORT_FUNC(UniformBufferCreate, uniformBufferCreate);

		// StorageBuffer
		MH_IMPORT_FUNC(StorageBufferCreate, storageBufferCreate);

		// ComputeShader
		MH_EXPORT_FUNC(ComputeShaderCreate, computeShaderCreate);

		// FrameBuffer
		MH_EXPORT_FUNC(FrameBufferCreate, framebufferCreate);

		// RenderBuffer
		MH_IMPORT_FUNC(RenderBufferCreate, renderBufferCreate);

		// Texture2D
		MH_IMPORT_FUNC(Texture2DCreateProps, tex2DCreateProps);
		MH_IMPORT_FUNC(Texture2DCreateFilepath, tex2DCreateFilepath);

		// TextureCube
		MH_IMPORT_FUNC(TextureCubeCreateProps, texCubeCreateProps);
		MH_IMPORT_FUNC(TextureCubeCreateFilepath, texCubeCreateFilepath);
		MH_IMPORT_FUNC(TextureCubeCreatePrefilter, texCubeCreatePrefilter);

		MH_CORE_ASSERT(i == NUM_FUNC_PTRS, "SharedLibrary function count mismatch. Change NUM_FUNC_PTRS to reflect the new count");
	}
}