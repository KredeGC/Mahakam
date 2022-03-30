#pragma once

#include "Buffer.h"
#include "FrameBuffer.h"
#include "RenderData.h"

#include <vector>
#include <robin_hood.h>

namespace Mahakam
{
	class RenderPass;

	class Renderer
	{
	private:
		struct RendererData
		{
			RendererResults rendererResults;
			std::vector<Ref<RenderPass>> renderPasses;

			Ref<FrameBuffer> gBuffer;
			Ref<FrameBuffer> viewportFramebuffer;
			Ref<Material> unlitMaterial;
			uint32_t width;
			uint32_t height;
		};

		static RendererData* rendererData;
		static SceneData* sceneData;

	public:
		static void Init(uint32_t width, uint32_t height);
		static void Shutdown();
		static void OnWindowResie(uint32_t width, uint32_t height);

		static void SetRenderPasses(const std::vector<Ref<RenderPass>>& renderPasses);

		static void BeginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment);
		static void EndScene();

		static void EnableWireframe(bool enable) { sceneData->wireframe = enable; }
		static void EnableBoundingBox(bool enable) { sceneData->boundingBox = enable; }
		static void EnableGBuffer(bool enable) { sceneData->gBuffer = enable; }

		static void Submit(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material);
		static void SubmitParticles(const glm::mat4& transform, const ParticleSystem& particles);

		static void DrawSkybox();
		static void DrawScreenQuad();
		static void DrawInstancedSphere(uint32_t amount);
		static void DrawInstancedPyramid(uint32_t amount);

		inline static bool HasWireframeEnabled() { return sceneData->wireframe; }
		inline static bool HasBoundingBoxEnabled() { return sceneData->boundingBox; }
		inline static bool HasGBufferEnabled() { return sceneData->gBuffer; }

		inline static Ref<FrameBuffer> GetGBuffer() { return rendererData->gBuffer; }
		inline static Ref<FrameBuffer> GetFrameBuffer() { return rendererData->viewportFramebuffer; }

		inline static void AddPerformanceResult(uint32_t vertexCount, uint32_t indexCount)
		{
			rendererData->rendererResults.drawCalls++;
			rendererData->rendererResults.vertexCount += vertexCount;
			rendererData->rendererResults.triCount += indexCount;
		}

		inline static const RendererResults& GetPerformanceResults() { return rendererData->rendererResults; }
	};
}