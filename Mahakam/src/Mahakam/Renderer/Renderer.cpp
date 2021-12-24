#include "mhpch.h"
#include "Renderer.h"

namespace Mahakam
{
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

	//std::vector<Renderer::MeshData> Renderer::renderQueue;

	std::unordered_map<Ref<Shader>, std::unordered_map<Ref<Material>, std::vector<Renderer::MeshData>>> Renderer::renderQueue;

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
		sceneData->viewMatrix = cam.getViewMatrix();
		sceneData->projectionMatrix = cam.getProjectionMatrix();

		renderQueue.clear();
	}
	
	void Renderer::endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
	{
		*drawCalls = 0;
		*vertexCount = 0;
		*triCount = 0;

		for (auto& shaderPair : renderQueue)
		{
			shaderPair.first->bind();
			shaderPair.first->setViewProjection(sceneData->viewMatrix, sceneData->projectionMatrix);

			for (auto& materialPair : shaderPair.second)
			{
				materialPair.first->bind();

				for (auto& data : materialPair.second)
				{
					materialPair.first->setTransform(data.transform);

					*drawCalls += 1;
					*vertexCount += data.mesh->getVertexCount();
					*triCount += data.mesh->getIndexCount();

					data.mesh->bind();

					GL::drawIndexed(data.mesh->getIndexCount());
				}
			}
		}

		/*for (auto& data : renderQueue)
		{
			*vertexCount += data.mesh->getVertexCount();
			*triCount += data.mesh->getIndexCount();

			data.mesh->getMaterial()->setMat4("u_MVP", sceneData->viewProjectionMatrix * data.transform);

			data.mesh->bind();

			GL::drawIndexed(data.mesh->getIndexCount());
		}*/

		*triCount /= 3;
	}
	
	void Renderer::submit(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		const Ref<Material>& material = mesh->getMaterial();
		const Ref<Shader>& shader = material->getShader();

		renderQueue[shader][material].push_back({ mesh, transform });

		//renderQueue.push_back({ mesh, transform });

		/*mesh->getMaterial()->setMat4("u_MVP", sceneData->viewProjectionMatrix * transform);

		mesh->bind();

		GL::drawIndexed(mesh->getIndexCount());*/
	}
}