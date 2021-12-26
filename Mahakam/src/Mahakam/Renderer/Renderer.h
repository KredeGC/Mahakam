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
		struct Matrices
		{
			glm::mat4 viewMatrix;
			glm::mat4 projectionMatrix;
		};

		struct SceneData
		{
			Ref<Camera> camera;
			std::vector<Ref<Light>> lights;
		};

		struct MeshData
		{
			float depth;
			Ref<Mesh> mesh;
			glm::mat4 transform;
		};

		static std::vector<glm::mat4> transformData;

		static SceneData* sceneData;

		static std::unordered_map<Ref<Shader>, // Sort by shader
			std::unordered_map<Ref<Material>, // Sort by material
			std::unordered_map<Ref<Mesh>, // Sort by mesh
			std::vector<glm::mat4>>>> renderQueue; // List of transforms for this mesh

		static std::vector<MeshData> transparentQueue;

	public:
		static void onWindowResie(uint32_t width, uint32_t height);
		static void init();

		static void beginScene(const Ref<Camera>& cam, const Ref<Light>& mainLight);
		static void endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount);

		static void submit(const glm::mat4& transform, const Ref<Mesh>& mesh);
		static void submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh);

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	};
}