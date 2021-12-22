#include "mhpch.h"
#include "Renderer.h"

namespace Mahakam
{
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

	void Renderer::onWindowResie(uint32_t width, uint32_t height)
	{
		GL::setViewport(0, 0, width, height);
	}

	void Renderer::init()
	{
		GL::init();
	}

	void Renderer::beginScene(OrthographicCamera& cam)
	{
		sceneData->viewProjectionMatrix = cam.getViewProjectionMatrix();
	}
	
	void Renderer::endScene()
	{
	
	}
	
	void Renderer::submit(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		mesh->getMaterial()->setMat4("u_MVP", sceneData->viewProjectionMatrix * transform);

		mesh->bind();

		GL::drawIndexed(mesh->getIndexCount());
	}
}