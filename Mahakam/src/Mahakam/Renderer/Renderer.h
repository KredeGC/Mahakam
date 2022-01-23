#pragma once

#include "GL.h"

#include "FrameBuffer.h"

#include "Camera.h"
#include "Mesh.h"
#include "Light.h"

#include <vector>

namespace Mahakam
{
	class Renderer
	{
	public:
		struct EnvironmentData
		{
			std::vector<Ref<Light>> lights;
			Ref<Texture> irradianceMap;
			Ref<Texture> specularMap;
		};

	private:
		struct SceneData
		{
			EnvironmentData environment;

			Ref<UniformBuffer> matrixBuffer;
			glm::mat4 viewProjectionMatrix;
			Ref<FrameBuffer> gBuffer;
			Ref<FrameBuffer> viewportFramebuffer;
			Ref<Material> lightingMaterial;
		};

		struct RendererResults
		{
			uint32_t drawCalls = 0;
			uint32_t vertexCount = 0;
			uint32_t triCount = 0;
		};

		struct MeshData
		{
			float depth;
			Ref<Mesh> mesh;
			Ref<Material> material;
			glm::mat4 transform;
		};

		static RendererResults* rendererResults;
		static SceneData* sceneData;

		static std::unordered_map<Ref<Shader>, // Sort by shader
			std::unordered_map<Ref<Material>, // Sort by material
			std::unordered_map<Ref<Mesh>, // Sort by mesh
			std::vector<glm::mat4>>>> renderQueue; // List of transforms for this mesh

		static std::vector<MeshData> transparentQueue;

	public:
		static void onWindowResie(uint32_t width, uint32_t height);
		static void init(uint32_t width, uint32_t height);

		static void beginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment);
		static void endScene();

		static void submit(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);
		static void submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);

		inline static Ref<FrameBuffer> getGBuffer() { return sceneData->gBuffer; }
		inline static Ref<FrameBuffer> getFrameBuffer() { return sceneData->viewportFramebuffer; }

		inline static const RendererResults* getPerformanceResults() { return rendererResults; }

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	};
}