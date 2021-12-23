#pragma once

#include "GL.h"

#include "Camera.h"
#include "Mesh.h"

namespace Mahakam
{
	class Renderer
	{
	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* sceneData;

	public:
		static void onWindowResie(uint32_t width, uint32_t height);
		static void init();

		static void beginScene(Camera& cam);
		static void endScene();

		static void submit(const glm::mat4& transform, const Ref<Mesh>& mesh);

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	};
}