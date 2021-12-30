#include "mhpch.h"
#include "Renderer.h"

namespace Mahakam
{
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;

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

	void Renderer::beginScene(const Ref<Camera>& cam, const Ref<Light>& mainLight)
	{
		sceneData->lights.clear();

		sceneData->camera = cam;
		sceneData->lights.push_back(mainLight);

		renderQueue.clear();
		transparentQueue.clear();
	}

	void Renderer::endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
	{
		Ref<UniformBuffer> lightBuffer = UniformBuffer::create(2 * 16);

		glm::vec3 lightPos = sceneData->lights[0]->getPosition();
		glm::vec3 lightCol = sceneData->lights[0]->getColor();
		lightBuffer->setData(&lightPos, 0, sizeof(glm::vec3));
		lightBuffer->setData(&lightCol, 16, sizeof(glm::vec3));

		*drawCalls = 0;
		*vertexCount = 0;
		*triCount = 0;

		sceneData->camera->getMatrixBuffer()->bind(0);
		lightBuffer->bind(1);

		for (auto& shaderPair : renderQueue)
		{
			shaderPair.first->bind();
			shaderPair.first->bindBuffer("Matrices", 0);
			shaderPair.first->bindBuffer("Lights", 1);

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

		if (transparentQueue.size() > 0)
		{
			GL::setBlendMode(true);
			for (auto& data : transparentQueue)
			{
				const Ref<Material>& material = data.mesh->getMaterial();
				const Ref<Shader>& shader = material->getShader();

				shader->bind();
				shader->bindBuffer("Matrices", 0);
				shader->bindBuffer("Lights", 1);
				material->bind();
				material->setTransform(data.transform);
				data.mesh->bind();

				*drawCalls += 1;
				*vertexCount += data.mesh->getVertexCount();
				*triCount += data.mesh->getIndexCount();

				GL::drawIndexed(data.mesh->getIndexCount());
			}
			GL::setBlendMode(false);
		}

		*triCount /= 3;
	}

	void Renderer::submit(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		const Ref<Material>& material = mesh->getMaterial();
		const Ref<Shader>& shader = material->getShader();

		renderQueue[shader][material][mesh].push_back(transform);
	}

	void Renderer::submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh)
	{
		float depth = (sceneData->camera->getViewProjectionMatrix() * transform[3]).z;

		transparentQueue.push_back({ depth, mesh, transform });
	}
}