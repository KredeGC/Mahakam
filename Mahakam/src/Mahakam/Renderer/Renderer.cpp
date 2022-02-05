#include "mhpch.h"
#include "Renderer.h"

#include "GL.h"

#include "RenderPasses.h"

#include <filesystem>
#include <sstream>
#include <fstream>


namespace Mahakam
{
	extern void InitRenderPasses(std::vector<RenderPass*>& renderPasses, uint32_t width, uint32_t height);

	Renderer::RendererResults* Renderer::rendererResults = new Renderer::RendererResults;
	Renderer::SceneData* Renderer::sceneData = new Renderer::SceneData;
	std::vector<RenderPass*> Renderer::renderPasses;

	Ref<FrameBuffer> Renderer::viewportFramebuffer;
	Ref<Material> Renderer::whiteMaterial;

	void Renderer::Init(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		GL::Init();

		// Initialize camera buffer
		sceneData->cameraBuffer = UniformBuffer::Create(sizeof(CameraData));

		// Initialize render passes
		InitRenderPasses(renderPasses, width, height);

		viewportFramebuffer = renderPasses[renderPasses.size() - 1]->GetFrameBuffer();

		// Initialize default material
		Ref<Shader> unlitColorShader = Shader::Create("assets/shaders/internal/UnlitColor.yaml");
		whiteMaterial = Material::Create(unlitColorShader);
		whiteMaterial->SetFloat3("u_Color", { 0.0f, 1.0f, 0.0f });
	}

	void Renderer::Shutdown()
	{
		GL::Shutdown();

		for (auto& renderPass : renderPasses)
			delete renderPass;

		viewportFramebuffer = nullptr;
		whiteMaterial = nullptr;

		delete rendererResults;
		delete sceneData;
	}

	void Renderer::OnWindowResie(uint32_t width, uint32_t height)
	{
		GL::SetViewport(0, 0, width, height);

		for (auto& renderPass : renderPasses)
			renderPass->OnWindowResize(width, height);
	}

	void Renderer::BeginScene(const Camera& cam, const glm::mat4& transform, const EnvironmentData& environment)
	{
		MH_PROFILE_FUNCTION();

		sceneData->environment = environment;

		// Setup camera matrices
		sceneData->cameraData = CameraData(cam, transform);

		sceneData->cameraBuffer->SetData(&sceneData->cameraData, 0, sizeof(CameraData));

		rendererResults->drawCalls = 0;
		rendererResults->vertexCount = 0;
		rendererResults->triCount = 0;
	}

	void Renderer::EndScene()
	{
		MH_PROFILE_FUNCTION();

		// Sort render queue
		std::qsort(sceneData->renderQueue.data(),
			sceneData->renderQueue.size(),
			sizeof(decltype(sceneData->renderQueue)::value_type),
			[](const void* x, const void* y) {
			const uint64_t arg1 = *static_cast<const uint64_t*>(x);
			const uint64_t arg2 = *static_cast<const uint64_t*>(y);

			const auto cmp = arg1 - arg2;

			if (cmp < 0) return -1;
			if (cmp > 0) return 1;
			return 0;
		});

		// Render each render pass
		Ref<FrameBuffer> prevBuffer = nullptr;
		for (uint32_t i = 0; i < renderPasses.size(); i++)
		{
			if (renderPasses[i]->Render(sceneData, prevBuffer))
				prevBuffer = renderPasses[i]->GetFrameBuffer();
		}

		// Render bounding boxes
		if (sceneData->boundingBox)
		{
			viewportFramebuffer->Bind();
			GL::SetFillMode(false);

			whiteMaterial->BindShader("GEOMETRY");
			whiteMaterial->Bind();

			for (uint64_t drawID : sceneData->renderQueue)
			{
				const uint64_t meshID = (drawID >> 16ULL) & 0xFFFFULL;
				Ref<Mesh>& mesh = sceneData->meshIDLookup[meshID];

				const uint64_t transformID = drawID & 0xFFFFULL;
				glm::mat4& transform = sceneData->transformIDLookup[transformID];

				auto& bounds = mesh->GetBounds();

				glm::vec3 positions[8] = {
					glm::vec3{ transform * glm::vec4{ bounds.positions[0], 1.0f } },
					glm::vec3{ transform * glm::vec4{ bounds.positions[1], 1.0f } },
					glm::vec3{ transform * glm::vec4{ bounds.positions[2], 1.0f } },
					glm::vec3{ transform * glm::vec4{ bounds.positions[3], 1.0f } },
					glm::vec3{ transform * glm::vec4{ bounds.positions[4], 1.0f } },
					glm::vec3{ transform * glm::vec4{ bounds.positions[5], 1.0f } },
					glm::vec3{ transform * glm::vec4{ bounds.positions[6], 1.0f } },
					glm::vec3{ transform * glm::vec4{ bounds.positions[7], 1.0f } }
				};

				Mesh::Bounds transformedBounds = Mesh::CalculateBounds(positions, 8);

				glm::vec3 scale = transformedBounds.max - transformedBounds.min;
				glm::vec3 center = transformedBounds.min + scale / 2.0f;

				glm::mat4 wireTransform = glm::translate(glm::mat4(1.0f), center)
					* glm::scale(glm::mat4(1.0f), scale);

				auto wireMesh = GL::GetCube();
				wireMesh->Bind();

				whiteMaterial->SetTransform(wireTransform);

				Renderer::AddPerformanceResult(wireMesh->GetVertexCount(), wireMesh->GetIndexCount());

				GL::DrawIndexed(wireMesh->GetIndexCount());
			}

			GL::SetFillMode(true);
			viewportFramebuffer->Unbind();
		}

		// Normalize results
		rendererResults->triCount /= 3;

		// Clear render queues
		sceneData->renderQueue.clear();

		sceneData->shaderRefLookup.clear();
		sceneData->materialRefLookup.clear();
		sceneData->meshRefLookup.clear();

		sceneData->shaderIDLookup.clear();
		sceneData->materialIDLookup.clear();
		sceneData->meshIDLookup.clear();
		sceneData->transformIDLookup.clear();
	}

	void Renderer::Submit(const glm::mat4& transform, Ref<Mesh> mesh, Ref<Material> material)
	{
		// TODO: View frustum culling using the mesh->GetBounds() method

		// Add shader if it doesn't exist
		uint64_t shaderID;
		Ref<Shader> shader = material->GetShader();
		auto& shaderIter = sceneData->shaderRefLookup.find(shader);
		if (shaderIter == sceneData->shaderRefLookup.end())
		{
			shaderID = sceneData->shaderRefLookup.size();
			sceneData->shaderRefLookup[shader] = shaderID;
			sceneData->shaderIDLookup[shaderID] = shader;
		}
		else
		{
			shaderID = shaderIter->second;
		}

		// Add material if it doesn't exist
		uint64_t materialID;
		auto& matIter = sceneData->materialRefLookup.find(material);
		if (matIter == sceneData->materialRefLookup.end())
		{
			materialID = sceneData->materialRefLookup.size();
			sceneData->materialRefLookup[material] = materialID;
			sceneData->materialIDLookup[materialID] = material;
		}
		else
		{
			materialID = matIter->second;
		}

		// Add mesh if it doesn't exist
		uint64_t meshID;
		auto& meshIter = sceneData->meshRefLookup.find(mesh);
		if (meshIter == sceneData->meshRefLookup.end())
		{
			meshID = sceneData->meshRefLookup.size();
			sceneData->meshRefLookup[mesh] = meshID;
			sceneData->meshIDLookup[meshID] = mesh;
		}
		else
		{
			meshID = meshIter->second;
		}

		// Add transform
		uint64_t transformID = sceneData->transformIDLookup.size();
		sceneData->transformIDLookup[transformID] = transform;

		uint64_t drawID = 0;

		if (false) // Fullscreen quad
		{
			drawID |= (3ULL << 62ULL);
		}
		else if (true) // Opaque or AlphaTest
		{
			if (false) // AlphaTest
				drawID |= (1ULL << 62ULL);

			drawID |= ((shaderID & 0x7FFFULL) << 47ULL);
			drawID |= ((materialID & 0x7FFFULL) << 32ULL);
			drawID |= ((meshID & 0xFFFFULL) << 16ULL);
			drawID |= (transformID & 0xFFFFULL);
		}
		else // Transparent
		{
			drawID |= (2ULL << 62ULL);

			float depth = (sceneData->cameraData.u_m4_VP * transform[3]).z;

			uint64_t depthInt = (uint64_t)(depth * ((1ULL << 31ULL) - 1ULL));

			drawID |= ((depthInt & 0xFFFFFFFFULL) << 30ULL);
		}

		sceneData->renderQueue.push_back(drawID);
	}

	void Renderer::DrawSkybox()
	{
		MH_PROFILE_FUNCTION();

		sceneData->environment.skyboxMaterial->BindShader("GEOMETRY");
		sceneData->environment.skyboxMaterial->Bind();
		DrawScreenQuad();
	}

	void Renderer::DrawScreenQuad()
	{
		rendererResults->drawCalls += 1;
		rendererResults->vertexCount += 4;
		rendererResults->triCount += 6;

		GL::DrawScreenQuad();
	}

	void Renderer::DrawInstancedSphere(uint32_t amount)
	{
		Ref<Mesh> invertedSphere = GL::GetInvertedSphere();

		rendererResults->drawCalls += 1;
		rendererResults->vertexCount += amount * invertedSphere->GetVertexCount();
		rendererResults->triCount += amount * invertedSphere->GetIndexCount();

		invertedSphere->Bind();

		GL::DrawInstanced(invertedSphere->GetIndexCount(), amount);
	}

	void Renderer::DrawInstancedPyramid(uint32_t amount)
	{
		Ref<Mesh> invertedPyramid = GL::GetInvertedPyramid();

		rendererResults->drawCalls += 1;
		rendererResults->vertexCount += amount * invertedPyramid->GetVertexCount();
		rendererResults->triCount += amount * invertedPyramid->GetIndexCount();

		invertedPyramid->Bind();

		GL::DrawInstanced(invertedPyramid->GetIndexCount(), amount);
	}

	//void Renderer::drawTransparentQueue()
	//{
	//	MH_PROFILE_FUNCTION();

	//	if (transparentQueue.size() > 0)
	//	{
	//		// Sort transparent queue
	//		std::qsort(transparentQueue.data(),
	//			transparentQueue.size(),
	//			sizeof(decltype(transparentQueue)::value_type),
	//			[](const void* x, const void* y) {
	//			const MeshData arg1 = *static_cast<const MeshData*>(x);
	//			const MeshData arg2 = *static_cast<const MeshData*>(y);

	//			const auto cmp = arg1.depth - arg2.depth;

	//			if (cmp < 0) return -1;
	//			if (cmp > 0) return 1;
	//			return 0;
	//		});

	//		GL::setBlendMode(RendererAPI::BlendMode::SrcAlpha, RendererAPI::BlendMode::OneMinusSrcAlpha, true);
	//		for (auto& data : transparentQueue)
	//		{
	//			const Ref<Material>& material = data.material;

	//			material->bindShader("FORWARD");
	//			material->bind();
	//			material->setTransform(data.transform);
	//			data.mesh->bind();

	//			rendererResults->drawCalls += 1;
	//			rendererResults->vertexCount += data.mesh->getVertexCount();
	//			rendererResults->triCount += data.mesh->getIndexCount();

	//			GL::drawIndexed(data.mesh->getIndexCount());
	//		}
	//		GL::setBlendMode(RendererAPI::BlendMode::One, RendererAPI::BlendMode::One, false);
	//	}
	//}
}