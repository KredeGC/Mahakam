#pragma once

#include "GL.h"

#include "Camera.h"
#include "Mesh.h"
#include "Light.h"

#include <vector>

namespace Mahakam
{
	class Renderer
	{
	private:
		struct SceneData
		{
			Ref<UniformBuffer> matrixBuffer;
			glm::mat4 viewProjectionMatrix;
			std::vector<Ref<Light>> lights;
		};

		struct MeshData
		{
			float depth;
			Ref<Mesh> mesh;
			Ref<Material> material;
			glm::mat4 transform;
		};

		static SceneData* sceneData;

		static std::unordered_map<Ref<Shader>, // Sort by shader
			std::unordered_map<Ref<Material>, // Sort by material
			std::unordered_map<Ref<Mesh>, // Sort by mesh
			std::vector<glm::mat4>>>> renderQueue; // List of transforms for this mesh

		static std::vector<MeshData> transparentQueue;

	public:
		static void onWindowResie(uint32_t width, uint32_t height);
		static void init();

		static void beginScene(const Camera& cam, const glm::mat4& transform, const Ref<Light>& mainLight);
		static void endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount);

		static void submit(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);
		static void submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	};
}