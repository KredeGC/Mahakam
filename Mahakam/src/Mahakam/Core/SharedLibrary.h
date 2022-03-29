#pragma once
#include "Core.h"

#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/ComputeShader.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/RenderBuffer.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	class SharedLibrary
	{
	public:
		// UniformBuffer
		MH_SHARED_FUNC(Ref<UniformBuffer>, UniformBufferCreate, uniformBufferCreate, uint32_t);

		// StorageBuffer
		MH_SHARED_FUNC(Ref<StorageBuffer>, StorageBufferCreate, storageBufferCreate, uint32_t);

		// ComputeShader
		MH_SHARED_FUNC(Ref<ComputeShader>, ComputeShaderCreate, computeShaderCreate, const std::string&);

		// FrameBuffer
		MH_SHARED_FUNC(Ref<FrameBuffer>, FrameBufferCreate, framebufferCreate, const FrameBufferProps&);

		// RenderBuffer
		MH_SHARED_FUNC(Ref<RenderBuffer>, RenderBufferCreate, renderBufferCreate, uint32_t, uint32_t, TextureFormat);

		// Texture2D
		MH_SHARED_FUNC(Ref<Texture2D>, Texture2DCreateProps, tex2DCreateProps, const TextureProps&);
		MH_SHARED_FUNC(Ref<Texture2D>, Texture2DCreateFilepath, tex2DCreateFilepath, const std::string&, const TextureProps&);

		// TextureCube
		MH_SHARED_FUNC(Ref<TextureCube>, TextureCubeCreateProps, texCubeCreateProps, const CubeTextureProps&);
		MH_SHARED_FUNC(Ref<TextureCube>, TextureCubeCreateFilepath, texCubeCreateFilepath, const std::string&, const CubeTextureProps&);
		MH_SHARED_FUNC(Ref<TextureCube>, TextureCubeCreatePrefilter, texCubeCreatePrefilter, Ref<TextureCube>, TextureCubePrefilter, const CubeTextureProps&);

		static constexpr int NUM_FUNC_PTRS = 10;

	private:
		const char* filepath = nullptr;
#if defined(MH_PLATFORM_WINDOWS)
		HINSTANCE handle = 0;
#else
		void* handle = nullptr;
#endif

		inline static bool initialized = false;
		inline static void* funcPointers[NUM_FUNC_PTRS];

	public:
		SharedLibrary() = default;

		SharedLibrary(const char* filepath);

		virtual ~SharedLibrary();

		static void ExportFuncPointers();
		static void ImportFuncPointers(void* ptrs[NUM_FUNC_PTRS]);

		template<typename R, typename ...Args>
		auto GetFunction(const char* name)
		{
#if defined(MH_PLATFORM_WINDOWS)
			return (R (*)(Args...))GetProcAddress(handle, name);
#elif defined(MH_PLATFORM_LINUX)
			return (R (*)(Args...))dlsym(myso, name);
#endif
		}
	};
}