#pragma once

#include "Mahakam/Core/SharedLibrary.h"

#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Asset/Asset.h"

#include <vector>
#include <string>

namespace Mahakam
{
	class Camera;
	class FrameBuffer;
	class Material;
	class SubMesh;
	class ParticleSystem;
	class RenderPass;

	struct EnvironmentData;
	struct SceneData;

	struct RendererResults
	{
		uint32_t DrawCalls = 0;
		uint32_t VertexCount = 0;
		uint32_t TriCount = 0;
	};

	class Renderer
	{
	private:
		using FrameBufferMap = UnorderedMap<std::string, Asset<FrameBuffer>>;

		struct RendererData
		{
			RendererResults FrameResults;
			std::vector<Ref<RenderPass>> RenderPasses;
			FrameBufferMap FrameBuffers;

			Asset<FrameBuffer> GBuffer;
			Asset<FrameBuffer> ViewportFramebuffer;
			uint32_t Width;
			uint32_t Height;
		};

		static Scope<RendererData> s_RendererData;
		static Scope<SceneData> s_SceneData;

	public:
		static void Init(uint32_t width, uint32_t height);
		static void Shutdown();

		inline static void OnWindowResize(uint32_t width, uint32_t height) { OnWindowResizeImpl(width, height); }
		
		inline static void SetRenderPasses(const std::vector<Ref<RenderPass>>& renderPasses) { SetRenderPassesImpl(renderPasses); }
		
		// Scenes
		inline static void BeginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment) { MH_PROFILE_FUNCTION(); BeginSceneImpl(cam, transform, environment); }
		inline static void EndScene() { MH_PROFILE_RENDERING_FUNCTION(); EndSceneImpl(); }
		
		inline static void Submit(const glm::mat4& transform, Ref<SubMesh> mesh, Asset<Material> material) { MH_PROFILE_FUNCTION(); SubmitImpl(transform, std::move(mesh), std::move(material)); }
		inline static void SubmitParticles(const glm::mat4& transform, const ParticleSystem& particles) { MH_PROFILE_FUNCTION(); SubmitParticlesImpl(transform, particles); }
		
		// Drawing
		inline static void DrawSkybox(SceneData* sceneData) { MH_PROFILE_FUNCTION(); DrawSkyboxImpl(sceneData); }
		inline static void DrawScreenQuad() { MH_PROFILE_FUNCTION(); DrawScreenQuadImpl(); }
		inline static void DrawInstancedSphere(uint32_t amount) { MH_PROFILE_FUNCTION(); DrawInstancedSphereImpl(amount); }
		inline static void DrawInstancedPyramid(uint32_t amount) { MH_PROFILE_FUNCTION(); DrawInstancedPyramidImpl(amount); }

		// Settings (TODO: Use a struct or delete)
		inline static void EnableWireframe(bool enable) { EnableWireframeImpl(enable); }
		inline static void EnableBoundingBox(bool enable) { EnableBoundingBoxImpl(enable); }
		inline static void EnableGBuffer(bool enable) { EnableGBufferImpl(enable); }

		inline static bool HasWireframeEnabled() { return HasWireframeEnabledImpl(); }
		inline static bool HasBoundingBoxEnabled() { return HasBoundingBoxEnabledImpl(); }
		inline static bool HasGBufferEnabled() { return HasGBufferEnabledImpl(); }

		// Framebuffers
		inline static void AddFrameBuffer(const std::string& name, Asset<FrameBuffer> frameBuffer) { AddFrameBufferImpl(name, std::move(frameBuffer)); }
		inline static const FrameBufferMap& GetFrameBuffers() { return GetFrameBuffersImpl(); }

		inline static Asset<FrameBuffer> GetGBuffer() { return GetGBufferImpl(); }
		inline static Asset<FrameBuffer> GetFrameBuffer() { return GetFrameBufferImpl(); }

		// Performance
		inline static void AddPerformanceResult(uint32_t vertexCount, uint32_t indexCount) { AddPerformanceResultImpl(vertexCount, indexCount); }
		inline static const RendererResults& GetPerformanceResults() { return GetPerformanceResultsImpl(); }

	private:
		MH_DECLARE_FUNC(OnWindowResizeImpl, void, uint32_t width, uint32_t height);

		MH_DECLARE_FUNC(SetRenderPassesImpl, void, const std::vector<Ref<RenderPass>>& renderPasses);
		 
		// Scenes
		MH_DECLARE_FUNC(BeginSceneImpl, void, const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment);
		MH_DECLARE_FUNC(EndSceneImpl, void);

		MH_DECLARE_FUNC(SubmitImpl, void, const glm::mat4& transform, Ref<SubMesh> mesh, Asset<Material> material);
		MH_DECLARE_FUNC(SubmitParticlesImpl, void, const glm::mat4& transform, const ParticleSystem& particles);
		
		// Drawing
		MH_DECLARE_FUNC(DrawSkyboxImpl, void, SceneData* sceneData);
		MH_DECLARE_FUNC(DrawScreenQuadImpl, void);
		MH_DECLARE_FUNC(DrawInstancedSphereImpl, void, uint32_t amount);
		MH_DECLARE_FUNC(DrawInstancedPyramidImpl, void, uint32_t amount);
		 
		// Settings
		MH_DECLARE_FUNC(EnableWireframeImpl, void, bool enable);
		MH_DECLARE_FUNC(EnableBoundingBoxImpl, void, bool enable);
		MH_DECLARE_FUNC(EnableGBufferImpl, void, bool enable);

		MH_DECLARE_FUNC(HasWireframeEnabledImpl, bool);
		MH_DECLARE_FUNC(HasBoundingBoxEnabledImpl, bool);
		MH_DECLARE_FUNC(HasGBufferEnabledImpl, bool);
		
		// Framebuffers
		MH_DECLARE_FUNC(AddFrameBufferImpl, void, const std::string& name, Asset<FrameBuffer> frameBuffer);
		MH_DECLARE_FUNC(GetFrameBuffersImpl, const FrameBufferMap&);
		
		MH_DECLARE_FUNC(GetGBufferImpl, Asset<FrameBuffer>);
		MH_DECLARE_FUNC(GetFrameBufferImpl, Asset<FrameBuffer>);
		 
		// Performance
		MH_DECLARE_FUNC(AddPerformanceResultImpl, void, uint32_t vertexCount, uint32_t indexCount);
		MH_DECLARE_FUNC(GetPerformanceResultsImpl, const RendererResults&);
	};
}