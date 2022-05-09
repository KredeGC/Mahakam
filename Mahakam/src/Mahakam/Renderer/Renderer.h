#pragma once

#include "Mahakam/Core/SharedLibrary.h"

#include "Buffer.h"
#include "FrameBuffer.h"
#include "RenderData.h"

#include <vector>
#include <robin_hood.h>

namespace Mahakam
{
	class RenderPass;

	// IDEA: Move the AABB visuals to a renderpass in Erebor, which can then be injected at will

	class Renderer
	{
	private:
		struct RendererData
		{
			RendererResults rendererResults;
			std::vector<Ref<RenderPass>> renderPasses;
			robin_hood::unordered_map<std::string, WeakRef<FrameBuffer>> frameBuffers;

			Asset<FrameBuffer> gBuffer;
			Asset<FrameBuffer> viewportFramebuffer;
			Asset<Material> unlitMaterial; // TEMP: For AABB visualization
			uint32_t width;
			uint32_t height;
		};

		using FrameBufferMap = robin_hood::unordered_map<std::string, WeakRef<FrameBuffer>>;

		static RendererData* rendererData;
		static SceneData* sceneData;

	public:
		static void Init(uint32_t width, uint32_t height);
		static void Shutdown();

		inline static void OnWindowResize(uint32_t width, uint32_t height) { OnWindowResizeImpl(width, height); }
		
		inline static void SetRenderPasses(const std::vector<Ref<RenderPass>>& renderPasses) { SetRenderPassesImpl(renderPasses); }
		
		// Scenes
		inline static void BeginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment) { BeginSceneImpl(cam, transform, environment); }
		inline static void EndScene() { EndSceneImpl(); }
		
		inline static void Submit(const glm::mat4& transform, Asset<Mesh> mesh, Asset<Material> material) { SubmitImpl(transform, mesh, material); }
		inline static void SubmitParticles(const glm::mat4& transform, const ParticleSystem& particles) { SubmitParticlesImpl(transform, particles); }
		
		// Drawing
		inline static void DrawSkybox() { DrawSkyboxImpl(); }
		inline static void DrawScreenQuad() { DrawScreenQuadImpl(); }
		inline static void DrawInstancedSphere(uint32_t amount) { DrawInstancedSphereImpl(amount); }
		inline static void DrawInstancedPyramid(uint32_t amount) { DrawInstancedPyramidImpl(amount); }

		// Settings (TODO: Use a struct or delete)
		inline static void EnableWireframe(bool enable) { EnableWireframeImpl(enable); }
		inline static void EnableBoundingBox(bool enable) { EnableBoundingBoxImpl(enable); }
		inline static void EnableGBuffer(bool enable) { EnableGBufferImpl(enable); }

		inline static bool HasWireframeEnabled() { return HasWireframeEnabledImpl(); }
		inline static bool HasBoundingBoxEnabled() { return HasBoundingBoxEnabledImpl(); }
		inline static bool HasGBufferEnabled() { return HasGBufferEnabledImpl(); }

		// Framebuffers
		inline static void AddFrameBuffer(const std::string& name, WeakRef<FrameBuffer> frameBuffer) { AddFrameBufferImpl(name, frameBuffer); }
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

		MH_DECLARE_FUNC(SubmitImpl, void, const glm::mat4& transform, Asset<Mesh> mesh, Asset<Material> material);
		MH_DECLARE_FUNC(SubmitParticlesImpl, void, const glm::mat4& transform, const ParticleSystem& particles);
		 
		// Drawing
		MH_DECLARE_FUNC(DrawSkyboxImpl, void);
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
		MH_DECLARE_FUNC(AddFrameBufferImpl, void, const std::string& name, WeakRef<FrameBuffer> frameBuffer);
		MH_DECLARE_FUNC(GetFrameBuffersImpl, const FrameBufferMap&);
		
		MH_DECLARE_FUNC(GetGBufferImpl, Asset<FrameBuffer>);
		MH_DECLARE_FUNC(GetFrameBufferImpl, Asset<FrameBuffer>);
		 
		// Performance
		MH_DECLARE_FUNC(AddPerformanceResultImpl, void, uint32_t vertexCount, uint32_t indexCount);
		MH_DECLARE_FUNC(GetPerformanceResultsImpl, const RendererResults&);
	};
}