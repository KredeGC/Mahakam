#include "mhpch.h"
#include "Renderer.h"

#include "Mahakam/Core/Application.h"

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
		MH_PROFILE_FUNCTION();

		GL::init();

		sceneData->matrixBuffer = UniformBuffer::create(sizeof(glm::vec3) + sizeof(glm::mat4) * 2);
	}

	void Renderer::beginScene(const Camera& cam, const glm::mat4& transform, const Ref<Light>& mainLight)
	{
		MH_PROFILE_FUNCTION();

		glm::mat4 viewMatrix = glm::inverse(transform);

		// Position
		sceneData->matrixBuffer->setData(&viewMatrix, 0, sizeof(glm::mat4));
		sceneData->matrixBuffer->setData(&transform[3], sizeof(glm::mat4) * 2, sizeof(glm::vec3));

		// Projection
		sceneData->matrixBuffer->setData(&cam.getProjectionMatrix(), sizeof(glm::mat4), sizeof(glm::mat4));

		sceneData->viewProjectionMatrix = cam.getProjectionMatrix() * viewMatrix;
		sceneData->lights.clear();
		sceneData->lights.push_back(mainLight);

		renderQueue.clear();
		transparentQueue.clear();
	}

	void Renderer::endScene(uint32_t* drawCalls, uint32_t* vertexCount, uint32_t* triCount)
	{
		MH_PROFILE_FUNCTION();

		//uint32_t width = Application::getInstance().getWindow().getWidth();
		//uint32_t height = Application::getInstance().getWindow().getHeight();

		//GL::setViewport(0, 0, width, height);

		Ref<UniformBuffer> lightBuffer = UniformBuffer::create(2 * 16);

		glm::vec3 lightPos = sceneData->lights[0]->getPosition();
		glm::vec3 lightCol = sceneData->lights[0]->getColor();
		lightBuffer->setData(&lightPos, 0, sizeof(glm::vec3));
		lightBuffer->setData(&lightCol, 16, sizeof(glm::vec3));

		*drawCalls = 0;
		*vertexCount = 0;
		*triCount = 0;

		sceneData->matrixBuffer->bind(0);
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
				const Ref<Material>& material = data.material;
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

	void Renderer::submit(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		MH_PROFILE_FUNCTION();

		const Ref<Shader>& shader = material->getShader();

		renderQueue[shader][material][mesh].push_back(transform);
	}

	void Renderer::submitTransparent(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		MH_PROFILE_FUNCTION();

		float depth = (sceneData->viewProjectionMatrix * transform[3]).z;

		transparentQueue.push_back({ depth, mesh, material, transform });
	}
}