#include "mhpch.h"
#include "Renderer.h"

namespace Mahakam
{
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

	std::vector<Renderer::MeshData> Renderer::renderQueue;

	void Renderer::onWindowResie(uint32_t width, uint32_t height)
	{
		GL::setViewport(0, 0, width, height);
	}

	void Renderer::init()
	{
		GL::init();
	}

	void Renderer::beginScene(Camera& cam)
	{
		sceneData->viewProjectionMatrix = cam.getViewProjectionMatrix();

		renderQueue.clear();
	}
	
	void Renderer::endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
	{
		*drawCalls = (uint32_t)renderQueue.size();
		*vertexCount = 0;
		*triCount = 0;

		for (auto& data : renderQueue)
		{
			*vertexCount += data.mesh->getVertexCount();
			*triCount += data.mesh->getIndexCount();

			data.mesh->getMaterial()->setMat4("u_MVP", sceneData->viewProjectionMatrix * data.transform);

			data.mesh->bind();

			GL::drawIndexed(data.mesh->getIndexCount());
		}

		*triCount /= 3;
	}
	
	void Renderer::submit(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		renderQueue.push_back({ mesh, transform });

		/*mesh->getMaterial()->setMat4("u_MVP", sceneData->viewProjectionMatrix * transform);

		mesh->bind();

		GL::drawIndexed(mesh->getIndexCount());*/
	}
}