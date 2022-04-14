#pragma once

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

			Ref<FrameBuffer> gBuffer;
			Ref<FrameBuffer> viewportFramebuffer;
			Ref<Material> unlitMaterial; // TEMP: For AABB visualization
			uint32_t width;
			uint32_t height;
		};

		RendererData rendererData;
		SceneData sceneData;

		static Renderer* s_Instance;

	public:
		Renderer(uint32_t width, uint32_t height);
		~Renderer();

		static Renderer* GetInstance();

		static void OnWindowResize(uint32_t width, uint32_t height);

		inline static void Init(uint32_t width, uint32_t height) { s_Instance = new Renderer(width, height); }
		inline static void Shutdown() { delete s_Instance; }
		
		inline static void SetRenderPasses(const std::vector<Ref<RenderPass>>& renderPasses) { GetInstance()->SetRenderPassesImpl(renderPasses); }
		
		// Scenes
		inline static void BeginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment) { GetInstance()->BeginSceneImpl(cam, transform, environment); }
		inline static void EndScene() { GetInstance()->EndSceneImpl(); }
		
		inline static void Submit(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material) { GetInstance()->SubmitImpl(transform, mesh, material); }
		inline static void SubmitParticles(const glm::mat4& transform, const ParticleSystem& particles) { GetInstance()->SubmitParticlesImpl(transform, particles); }
		
		// Drawing
		inline static void DrawSkybox() { GetInstance()->DrawSkyboxImpl(); }
		inline static void DrawScreenQuad() { GetInstance()->DrawScreenQuadImpl(); }
		inline static void DrawInstancedSphere(uint32_t amount) { GetInstance()->DrawInstancedSphereImpl(amount); }
		inline static void DrawInstancedPyramid(uint32_t amount) { GetInstance()->DrawInstancedPyramidImpl(amount); }

		// Settings (TODO: Use a struct or delete)
		inline static void EnableWireframe(bool enable) { GetInstance()->EnableWireframeImpl(enable); }
		inline static void EnableBoundingBox(bool enable) { GetInstance()->EnableBoundingBoxImpl(enable); }
		inline static void EnableGBuffer(bool enable) { GetInstance()->EnableGBufferImpl(enable); }

		inline static bool HasWireframeEnabled() { return GetInstance()->HasWireframeEnabledImpl(); }
		inline static bool HasBoundingBoxEnabled() { return GetInstance()->HasBoundingBoxEnabledImpl(); }
		inline static bool HasGBufferEnabled() { return GetInstance()->HasGBufferEnabledImpl(); }

		// Framebuffers
		inline static void AddFrameBuffer(const std::string& name, WeakRef<FrameBuffer> frameBuffer) { GetInstance()->AddFrameBufferImpl(name, frameBuffer); }
		inline static const robin_hood::unordered_map<std::string, WeakRef<FrameBuffer>>& GetFrameBuffers() { return GetInstance()->GetFrameBuffersImpl(); }

		inline static Ref<FrameBuffer> GetGBuffer() { return GetInstance()->GetGBufferImpl(); }
		inline static Ref<FrameBuffer> GetFrameBuffer() { return GetInstance()->GetFrameBufferImpl(); }

		// Performance
		inline static void AddPerformanceResult(uint32_t vertexCount, uint32_t indexCount) { GetInstance()->AddPerformanceResultImpl(vertexCount, indexCount); }
		inline static const RendererResults& GetPerformanceResults() { return GetInstance()->GetPerformanceResultsImpl(); }

	private:
		void SetRenderPassesImpl(const std::vector<Ref<RenderPass>>& renderPasses);

		// Scenes
		void BeginSceneImpl(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment);
		void EndSceneImpl();

		void SubmitImpl(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material);
		void SubmitParticlesImpl(const glm::mat4& transform, const ParticleSystem& particles);

		// Drawing
		void DrawSkyboxImpl();
		void DrawScreenQuadImpl();
		void DrawInstancedSphereImpl(uint32_t amount);
		void DrawInstancedPyramidImpl(uint32_t amount);

		// Settings
		inline void EnableWireframeImpl(bool enable) { sceneData.wireframe = enable; }
		inline void EnableBoundingBoxImpl(bool enable) { sceneData.boundingBox = enable; }
		inline void EnableGBufferImpl(bool enable) { sceneData.gBuffer = enable; }

		inline bool HasWireframeEnabledImpl() { return sceneData.wireframe; }
		inline bool HasBoundingBoxEnabledImpl() { return sceneData.boundingBox; }
		inline bool HasGBufferEnabledImpl() { return sceneData.gBuffer; }

		// Framebuffers
		void AddFrameBufferImpl(const std::string& name, WeakRef<FrameBuffer> frameBuffer);
		const robin_hood::unordered_map<std::string, WeakRef<FrameBuffer>>& GetFrameBuffersImpl();

		inline Ref<FrameBuffer> GetGBufferImpl() { return rendererData.gBuffer; }
		inline Ref<FrameBuffer> GetFrameBufferImpl() { return rendererData.viewportFramebuffer; }

		// Performance
		inline void AddPerformanceResultImpl(uint32_t vertexCount, uint32_t indexCount)
		{
			rendererData.rendererResults.drawCalls++;
			rendererData.rendererResults.vertexCount += vertexCount;
			rendererData.rendererResults.triCount += indexCount;
		}

		inline const RendererResults& GetPerformanceResultsImpl() { return rendererData.rendererResults; }
	};
}