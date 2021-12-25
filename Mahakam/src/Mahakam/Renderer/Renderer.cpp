#include "mhpch.h"
#include "Renderer.h"

namespace Mahakam
{
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

	std::vector<glm::mat4> Renderer::transformData;

	std::unordered_map<Ref<Shader>,
		std::unordered_map<Ref<Material>,
		std::unordered_map<Ref<Mesh>,
		std::vector<glm::mat4>>>> Renderer::renderQueue;

	std::vector<Renderer::MeshData> Renderer::transparentQueue;

	void Renderer::onWindowResie(uint32_t width, uint32_t height)
	{
		GL::setViewport(0, 0, width, height);
	}

	void Renderer::init()
	{
		GL::init();
	}

	void Renderer::beginScene(const Ref<Camera>& cam)
	{
		sceneData->camera = cam;

		//sceneData->viewMatrix = cam.getViewMatrix();
		//sceneData->projectionMatrix = cam.getProjectionMatrix();

		renderQueue.clear();
		transformData.clear();
	}

	inline size_t uboAligned(size_t size) { return ((size + 255) / 256) * 256; }

	void Renderer::endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
	{
		//Ref<UniformBuffer> transformBuffer = UniformBuffer::create(2 * 79 * uboAligned(sizeof(glm::mat4)));

		*drawCalls = 0;
		*vertexCount = 0;
		*triCount = 0;

		/*for (int i = 0; i < 2 * 79; i++)
		{
			transformBuffer->setData(&transformData[i], i * uboAligned(sizeof(glm::mat4)), sizeof(glm::mat4));
		}*/

		sceneData->camera->getMatrixBuffer()->bind(0);

		for (auto& shaderPair : renderQueue)
		{
			shaderPair.first->bind();
			shaderPair.first->bindBuffer("Matrices", 0);
			//shaderPair.first->bindBuffer("Transform", 1);

			for (auto& materialPair : shaderPair.second)
			{
				// TODO: Use a UBO instead of rebinding the uniforms each time
				materialPair.first->bind();

				// TODO: Use batching at this point?
				for (auto& meshPair : materialPair.second)
				{
					meshPair.first->bind();
					for (auto& transform : meshPair.second)
					{
						//transformBuffer->bind(1, (*drawCalls) * uboAligned(sizeof(glm::mat4)), sizeof(glm::mat4));

						materialPair.first->setTransform(transform);

						*drawCalls += 1;
						*vertexCount += meshPair.first->getVertexCount();
						*triCount += meshPair.first->getIndexCount();

						GL::drawIndexed(meshPair.first->getIndexCount());
					}
				}
			}
		}

		std::qsort(transparentQueue.data(),
			transparentQueue.size(),
			sizeof(decltype(transparentQueue)::value_type),
			[](const void* x, const void* y) {
			const MeshData arg1 = *static_cast<const MeshData*>(x);
			const MeshData arg2 = *static_cast<const MeshData*>(y);

			const auto cmp = arg1.depth - arg2.depth;

			if (cmp < 0) return -1;
			if (cmp > 0) return 1;
			return 0;
		});

		*triCount /= 3;
	}

	void Renderer::submit(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		const Ref<Material>& material = mesh->getMaterial();
		const Ref<Shader>& shader = material->getShader();

		renderQueue[shader][material][mesh].push_back(transform);

		transformData.push_back(transform);
	}

	void Renderer::submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		float depth = (sceneData->camera->getViewProjectionMatrix() * transform[3]).z;

		transparentQueue.push_back({ depth, mesh, transform });
	}
}