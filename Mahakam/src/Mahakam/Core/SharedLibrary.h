#pragma once
#include "Core.h"

#include "Mahakam/Renderer/Animation.h"
#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/ComputeShader.h"
#include "Mahakam/Renderer/FrameBuffer.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Mesh.h"
#include "Mahakam/Renderer/RenderBuffer.h"
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	class SharedLibrary
	{
	public:
		// Animation
		MH_SHARED_FUNC(Ref<Animation>, AnimationLoad, const std::string&, SkinnedMesh&);

		// UniformBuffer
		MH_SHARED_FUNC(Ref<UniformBuffer>, UniformBufferCreate, uint32_t);

		// StorageBuffer
		MH_SHARED_FUNC(Ref<StorageBuffer>, StorageBufferCreate, uint32_t);

		// ComputeShader
		MH_SHARED_FUNC(Ref<ComputeShader>, ComputeShaderCreate, const std::string&);

		// FrameBuffer
		MH_SHARED_FUNC(Ref<FrameBuffer>, FrameBufferCreate, const FrameBufferProps&);

		// TODO: GL

		// Log
		MH_SHARED_FUNC(Ref<spdlog::logger>&, EngineLogger);
		MH_SHARED_FUNC(Ref<spdlog::logger>&, GameLogger);

		// Material
		MH_SHARED_FUNC(Ref<Material>, MaterialCopy, Ref<Material>);
		MH_SHARED_FUNC(Ref<Material>, MaterialCreate, Ref<Shader>, const std::string&);

		// Mesh
		MH_SHARED_FUNC(Ref<Mesh>, MeshCreate, uint32_t, uint32_t, void* verts[Mesh::BUFFER_ELEMENTS_SIZE], const uint32_t*);
		MH_SHARED_FUNC(SkinnedMesh, MeshLoad, const std::string&, const SkinnedMeshProps&);

		// Profiler
		MH_SHARED_FUNC(Profiler, ProfilerCreate, const char*, bool);
		MH_SHARED_FUNC(void, ProfilerAddResult, const char*, std::chrono::time_point<std::chrono::steady_clock>, std::chrono::time_point<std::chrono::steady_clock>);
		MH_SHARED_FUNC(void, ProfilerClear);
		MH_SHARED_FUNC(const std::vector<Profiler::ProfileResult>&, ProfilerGetResults);

		// RenderBuffer
		MH_SHARED_FUNC(Ref<RenderBuffer>, RenderBufferCreate, uint32_t, uint32_t, TextureFormat);

		// TODO: Renderer

		// Shader
		MH_SHARED_FUNC(Ref<Shader>, ShaderCreate, const std::string&, const std::initializer_list<std::string>&);

		// Texture2D
		MH_SHARED_FUNC(Ref<Texture2D>, Texture2DCreateProps, const TextureProps&);
		MH_SHARED_FUNC(Ref<Texture2D>, Texture2DCreateFilepath, const std::string&, const TextureProps&);

		// TextureCube
		MH_SHARED_FUNC(Ref<TextureCube>, TextureCubeCreateProps, const CubeTextureProps&);
		MH_SHARED_FUNC(Ref<TextureCube>, TextureCubeCreateFilepath, const std::string&, const CubeTextureProps&);
		MH_SHARED_FUNC(Ref<TextureCube>, TextureCubeCreatePrefilter, Ref<TextureCube>, TextureCubePrefilter, const CubeTextureProps&);

		static constexpr int NUM_FUNC_PTRS = 22;

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