#pragma once

#include "GL.h"

#include "Camera.h"
#include "Mesh.h"

#include <vector>

namespace Mahakam
{
	class Renderer
	{
	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		struct MeshData
		{
			Ref<Mesh> mesh;
			glm::mat4 transform;
		};

		static SceneData* sceneData;

		static std::vector<MeshData> renderQueue;

	public:
		static void onWindowResie(uint32_t width, uint32_t height);
		static void init();

		static void beginScene(Camera& cam);
		static void endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount);

		static void submit(const glm::mat4& transform, const Ref<Mesh>& mesh);

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	};
}